
#include "NetCorePch.h"
#include "NetCore.h"
#include "Session.h"
#include "NetMessageQueue.h"
#include "SessionManager.h"


#define NET_CORE_NUM_MAX_SESSIONS (1024)

static LONG s_isAllocated = 0;
NetCore* pNetCore;


struct AcceptIoOperationData
{
	IoOperationData data;
	CHAR buffer[2 * (sizeof(SOCKADDR_IN) + 16)];
};

static WSADATA s_wsa = { 0, };

static HANDLE s_hIOCP;
static SOCKET s_hListeningSocket;
static AcceptIoOperationData s_acceptIoOperationData;


static HANDLE s_ioThreads[NET_CORE_NUM_THREADS];

static SRWLOCK s_sessionEventLocks[NET_CORE_NUM_THREADS];
static NetSessionEventQueue* s_pSessionEventsFront;
static NetSessionEventQueue* s_pSessionEventsBack;

static SRWLOCK s_receiveSrwLocks[NET_CORE_NUM_THREADS];
static NetMessageQueue* s_pReceiveMessagesFront;
static NetMessageQueue* s_pReceiveMessagesBack;

static SessionManager s_sessionManager;


unsigned WINAPI ThreadIoCompletion(LPVOID pParam);

void OnAccept(AcceptIoOperationData* data, int threadId);
bool PostAccept();

void InitiateAllocation();
void TerminateAllocation();

void s_WriteSessionEvent(UINT32 sessionId, ESessionEvent sessionEvent, int threadId);


bool NetCore::StartNetCore()
{
	if (WSAStartup(MAKEWORD(2, 2), &s_wsa))
	{
		perror("Error. Winsock을 초기화 할 수 없습니다.");
		return false;
	}

	s_sessionManager.Initiate();

	s_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, NET_CORE_NUM_THREADS);
	if (s_hIOCP == NULL) {
		puts("Error: IOCP를 생성할 수 없습니다.");
		return false;
	}

	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		s_ioThreads[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadIoCompletion, NULL, 0, NULL);
		// Todo: begin thread가 실패하는 경우 
	}

	InitiateAllocation();

	return true;
}

void NetCore::StartAccept()
{
	s_hListeningSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN serverAddr = { 0, };
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(NET_CORE_PORT);

	if (bind(s_hListeningSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		puts("bind() error.");
		EndNetCore();
		return;
	}

	if (listen(s_hListeningSocket, SOMAXCONN) == SOCKET_ERROR) {
		puts("listen() error");
		EndNetCore();
		return;
	}

	if (CreateIoCompletionPort((HANDLE)s_hListeningSocket, s_hIOCP, 0, 0) == NULL) {
		puts("IOCP Create Fail");
		EndNetCore();
		return;
	}

	PostAccept();
}

void NetCore::EndNetCore()
{
	s_sessionManager.DisconnectAllSessions();

	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		PostQueuedCompletionStatus(s_hIOCP, 0, NULL, NULL);
	}

	WaitNetCore();

	shutdown(s_hListeningSocket, SD_BOTH);
	closesocket(s_hListeningSocket);
	s_hListeningSocket = NULL;

	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		CloseHandle(s_ioThreads[i]);
	}

	s_sessionManager.Terminate();
	TerminateAllocation();

	CloseHandle(s_hIOCP);
	s_hIOCP = NULL;

	WSACleanup();

	
	LONG isAllocated = InterlockedExchange(&s_isAllocated, 0);
	if (isAllocated) {
		delete pNetCore;
	}
}

UINT32 NetCore::ConnectTo(const char* ip, int port)
{
	// Todo: Async Connect 로 변경
	// Todo: 실패 시 전역 Error code 반환
	SOCKADDR_IN connAddr;
	ZeroMemory(&connAddr, sizeof(connAddr));

	connAddr.sin_family = AF_INET;
	connAddr.sin_addr.s_addr = inet_addr(ip);
	connAddr.sin_port = htons(port);

	SOCKET hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (hSocket == INVALID_SOCKET) {
		__debugbreak();
		return 0;
	}

	int res = connect(hSocket, (SOCKADDR*)&connAddr, sizeof(connAddr));
	if (res == SOCKET_ERROR) {
		__debugbreak();
		auto error = WSAGetLastError();
		closesocket(hSocket);
		return 0;
	}

	Session* pSession = s_sessionManager.CreateSession(hSocket);

	if (CreateIoCompletionPort((HANDLE)hSocket, s_hIOCP, (ULONG_PTR)pSession, 0) == NULL) {
		puts("IOCP Create Fail");
		EndNetCore();
		return 0;
	}

	pSession->RegisterReceive();

	return pSession->GetID();
}

void NetCore::Disconnect(UINT32 sessionID)
{
	s_sessionManager.RemoveSession(sessionID, ESessionRemoveReason::FORCE_REMOVE);
}

void NetCore::WaitNetCore()
{
	
	WaitForMultipleObjects(NET_CORE_NUM_THREADS, s_ioThreads, TRUE, INFINITE);
	printf("All Threads end\n");
	
}

bool NetCore::SendMessageTo(UINT32 sessionID, BYTE* msg, UINT32 length)
{
	assert(length < NET_MSG_MAX_LEN - sizeof(NetMessage::header));

	s_sessionManager.SendMessageTo(sessionID, msg, length);

	return true;
}

bool NetCore::SendMessageTo(UINT32* sessionIDs, UINT32 numSessions, BYTE* msg, UINT32 length)
{
	for (UINT32 i = 0; i < numSessions; ++i) {
		bool res = SendMessageTo(sessionIDs[i], msg, length);
		// Todo: 일부 실패상황 대응
		if (!res) {
			__debugbreak();
		}
	}
	return true;
}

NetSessionEventQueue* NetCore::StartHandleSessionEvents()
{
	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		AcquireSRWLockExclusive(&s_sessionEventLocks[i]);
	}

	NetSessionEventQueue* tmp = s_pSessionEventsFront;
	s_pSessionEventsFront = s_pSessionEventsBack;
	s_pSessionEventsBack = tmp;

	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		ReleaseSRWLockExclusive(&s_sessionEventLocks[i]);
	}

	return s_pSessionEventsFront;
}

void NetCore::EndHandleSessionEvents()
{
	s_pSessionEventsFront->Flush();
}

NetMessageQueue* NetCore::StartHandleReceivedMessages()
{
	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		AcquireSRWLockExclusive(&s_receiveSrwLocks[i]);
	}

	NetMessageQueue* tmp = s_pReceiveMessagesFront;
	s_pReceiveMessagesFront = s_pReceiveMessagesBack;
	s_pReceiveMessagesBack = tmp;


	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		ReleaseSRWLockExclusive(&s_receiveSrwLocks[i]);
	}

	return s_pReceiveMessagesFront;
}

void NetCore::EndHandleReceivedMessages()
{
	s_pReceiveMessagesFront->Flush();
}

void NetCore::EndIoThreads()
{
	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		PostQueuedCompletionStatus(s_hIOCP, 0, 0, NULL);
	}
}




unsigned WINAPI ThreadIoCompletion(LPVOID pParam)
{
	static LONG s_threadID = 0;

	unsigned endFlag = 0;

	DWORD               dwTransferredSize = 0;
	DWORD               dwFlag = 0;
	Session*			pSession = NULL;
	LPWSAOVERLAPPED     pOverlapped = NULL;
	BOOL                result = false;

	int threadID = InterlockedIncrement(&s_threadID) - 1;

	s_receiveSrwLocks[threadID] = SRWLOCK_INIT;
	s_sessionEventLocks[threadID] = SRWLOCK_INIT;

	NetPage* netPage = NULL;
	printf("Start IO Completion thread[%d]..\n", threadID);
	while (TRUE) {
		result = GetQueuedCompletionStatus(s_hIOCP, &dwTransferredSize, (PULONG_PTR)&pSession, &pOverlapped, INFINITE);
		if (result)
		{
			if (pOverlapped == NULL) {
				printf("End IO Thread [%d]\n", threadID);
				goto END_THREAD;
			}

			// 정상 실행
			IoOperationData* pData = CONTAINING_RECORD(pOverlapped, IoOperationData, wol);
			if (pData->operation == IoOperationData::RECEIVE)
			{
				// recieve 완료
				if (dwTransferredSize == 0)
				{
					// 클라이언트의 종료
					ESessionRefResult res = pSession->ReduceReference(ESessionRefParam::SESSION_REF_DECREASE_RECV);
					if (res == SESSION_REF_DEACTIVATE) {
						SHORT id = pSession->GetID();
						s_sessionManager.RemoveSession(id, COMPLETION_RECV_ERROR);
						s_WriteSessionEvent(id, ESessionEvent::DELETE_PASSIVE_CLIENT, threadID);
					}
				}
				else
				{
					// 데이터를 수신한 경우
					pSession->OnReceive(dwTransferredSize);
					
					NetMessage* pNetMessage = pSession->GetReceiveNetMessageOrNull();
					AcquireSRWLockExclusive(&s_receiveSrwLocks[threadID]);
					while (pNetMessage != NULL) {
						assert(pNetMessage->header.length != 0);
						s_pReceiveMessagesBack->WriteNetMessage(pSession->GetID(), pNetMessage);
						pNetMessage = pSession->GetReceiveNetMessageOrNull();
					}
					ReleaseSRWLockExclusive(&s_receiveSrwLocks[threadID]);
					
					pSession->RegisterReceive();
				}
			}
			else if (pData->operation == IoOperationData::SEND)
			{
				pSession->OnSendComplete();
			}
			else if (pData->operation == IoOperationData::ACCEPT)
			{
				AcceptIoOperationData* accpetIoOpData = (AcceptIoOperationData*)pOverlapped;
				OnAccept(accpetIoOpData, threadID);

				
			}
		}
		else
		{
			if (pSession == NULL || pOverlapped == NULL) {
				// 발생하면 안되는 경우
				__debugbreak();
			}

			IoOperationData* pData = CONTAINING_RECORD(pOverlapped, IoOperationData, wol);
			if (pData->operation == IoOperationData::RECEIVE) {
				ESessionRefResult res = pSession->ReduceReference(ESessionRefParam::SESSION_REF_DECREASE_RECV);
				if (res == SESSION_REF_DEACTIVATE) {
					SHORT id = pSession->GetID();
					s_sessionManager.RemoveSession(id, COMPLETION_RECV_ERROR);
					s_WriteSessionEvent(id, ESessionEvent::DELETE_PASSIVE_CLIENT, threadID);
				}
				continue;
			}
			else if (pData->operation == IoOperationData::SEND) {
				ESessionRefResult res = pSession->ReduceReference(ESessionRefParam::SESSION_REF_DECREASE_SEND);
				if (res == SESSION_REF_DEACTIVATE) {
					SHORT id = pSession->GetID();
					s_sessionManager.RemoveSession(id, COMPLETION_SEND_ERROR);
					s_WriteSessionEvent(id, ESessionEvent::DELETE_PASSIVE_CLIENT, threadID);
				}
				continue;
			}
			else {
				__debugbreak();
			}

			puts("GQCS 비정상 종료");
			endFlag = 1;
			goto END_THREAD;
			
		}
	}

END_THREAD:
	// free allocated memorys

	return endFlag;
}


void OnAccept(AcceptIoOperationData* data, int threadId)
{
	Session* pNewSession = s_sessionManager.CreateSession(data->data.socket);

	CreateIoCompletionPort(
		(HANDLE)pNewSession->GetSocket(),
		s_hIOCP,
		(ULONG_PTR)pNewSession,
		0
	);

	pNewSession->RegisterReceive();

	s_WriteSessionEvent(pNewSession->GetID(), ESessionEvent::CREATE_PASSIVE_CLIENT, threadId);

	// Register async accept chain
	PostAccept();
}

bool PostAccept()
{
	SOCKET acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (acceptSocket == INVALID_SOCKET) {
		puts("accept socket creation fail");
		return false;
	}

	s_acceptIoOperationData.data.socket = acceptSocket;
	ZeroMemory(&s_acceptIoOperationData.data.wol, sizeof(OVERLAPPED));
	s_acceptIoOperationData.data.operation = IoOperationData::ACCEPT;

	DWORD bytesReceived = 0;
	BOOL result = AcceptEx(
		s_hListeningSocket,						// 연결을 수락하는 소켓
		s_acceptIoOperationData.data.socket,		// 수락 시 연결되는 소켓
		s_acceptIoOperationData.buffer,			// 클라이언트와 서버의 주소를 저장할 버퍼
		0,											// 초기 데이터 길이
		sizeof(SOCKADDR_IN) + 16,				// 로컬 주소를 저장할 버퍼의 길이
		sizeof(SOCKADDR_IN) + 16,				// 원격 주소 정보를 저장할 버퍼 길이
		&bytesReceived,
		&s_acceptIoOperationData.data.wol
	);

	if (!result && WSAGetLastError() != ERROR_IO_PENDING) {
		puts("AcceptEx Failed");
		return false;
	}

	return true;
}

void InitiateAllocation()
{
	s_pReceiveMessagesFront = DNew NetMessageQueue();
	s_pReceiveMessagesBack = DNew NetMessageQueue();

	s_pSessionEventsFront = DNew NetSessionEventQueue();
	s_pSessionEventsBack = DNew NetSessionEventQueue();
}

void TerminateAllocation()
{
	delete s_pSessionEventsBack;
	delete s_pSessionEventsFront;
	delete s_pReceiveMessagesFront;
	delete s_pReceiveMessagesBack;
}

void s_WriteSessionEvent(UINT32 sessionId, ESessionEvent sessionEvent, int threadId)
{
	AcquireSRWLockExclusive(&s_sessionEventLocks[threadId]);
	s_pSessionEventsBack->WriteNetSessionEvent(sessionId, sessionEvent);
	ReleaseSRWLockExclusive(&s_sessionEventLocks[threadId]);
}

NETCORE_API NetCore* GetNetCore()
{
	LONG alloc = InterlockedExchange(&s_isAllocated, 1);
	if (alloc == 1) {
		return pNetCore;
	}

	pNetCore = DNew NetCore();
	return pNetCore;
}

NETCORE_API void DeleteNetCore()
{
	LONG alloc = InterlockedExchange(&s_isAllocated, 0);
	if (alloc == 0) {
		__debugbreak();
		return;
	}

	delete pNetCore;
}

NETCORE_API void CreateNetCore(NetCore** dst)
{
	*dst = DNew NetCore;
}

NETCORE_API void DeleteNetCore(NetCore* src)
{
	delete src;
}
