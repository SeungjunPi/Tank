
#include "NetCorePch.h"
#include "NetCore.h"
#include "Session.h"
#include "NetMessageQueue.h"
#include "NetSessionEventQueue.h"
#include "SessionManager.h"


#define NET_CORE_NUM_MAX_SESSIONS (1024)



bool NetCore::StartNetCore()
{
	if (WSAStartup(MAKEWORD(2, 2), &_wsa))
	{
		perror("Error. Winsock을 초기화 할 수 없습니다.");
		return false;
	}

	_sessionManager.Initiate();

	_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, NET_CORE_NUM_THREADS);
	if (_hIOCP == NULL) {
		puts("Error: IOCP를 생성할 수 없습니다.");
		return false;
	}

	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		_threadArgs[i].pNetCore = this;
		_threadArgs[i].threadID = i;


		_receiveSrwLocks[i] = SRWLOCK_INIT;
		_sessionEventLocks[i] = SRWLOCK_INIT;
		
		_ioThreads[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadIoCompletion, _threadArgs + i, 0, NULL);
		// Todo: begin thread가 실패하는 경우 
	}

	InitiateAllocation();

	return true;
}

void NetCore::StartAccept()
{
	_hListeningSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN serverAddr = { 0, };
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(NET_CORE_PORT);

	if (bind(_hListeningSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		puts("bind() error.");
		EndNetCore();
		return;
	}

	if (listen(_hListeningSocket, SOMAXCONN) == SOCKET_ERROR) {
		puts("listen() error");
		EndNetCore();
		return;
	}

	if (CreateIoCompletionPort((HANDLE)_hListeningSocket, _hIOCP, 0, 0) == NULL) {
		puts("IOCP Create Fail");
		EndNetCore();
		return;
	}

	PostAccept();
}

void NetCore::EndNetCore()
{
	_sessionManager.DisconnectAllSessions();

	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		PostQueuedCompletionStatus(_hIOCP, 0, NULL, NULL);
	}

	WaitNetCore();

	shutdown(_hListeningSocket, SD_BOTH);
	closesocket(_hListeningSocket);
	_hListeningSocket = NULL;

	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		CloseHandle(_ioThreads[i]);
	}

	_sessionManager.Terminate();
	TerminateAllocation();

	CloseHandle(_hIOCP);
	_hIOCP = NULL;

	WSACleanup();
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

	Session* pSession = _sessionManager.CreateSession(hSocket);

	if (CreateIoCompletionPort((HANDLE)hSocket, _hIOCP, (ULONG_PTR)pSession, 0) == NULL) {
		puts("IOCP Create Fail");
		EndNetCore();
		return 0;
	}

	pSession->RegisterReceive();

	return pSession->GetID();
}

void NetCore::Disconnect(UINT32 sessionID)
{
	_sessionManager.RemoveSession(sessionID, ESessionRemoveReason::FORCE_REMOVE);
}

void NetCore::WaitNetCore()
{
	
	WaitForMultipleObjects(NET_CORE_NUM_THREADS, _ioThreads, TRUE, INFINITE);
	printf("All Threads end\n");
	
}

bool NetCore::SendMessageTo(UINT32 sessionID, BYTE* msg, UINT32 length)
{
	assert(length < NET_MSG_MAX_LEN - sizeof(NetMessage::header));

	_sessionManager.SendMessageTo(sessionID, msg, length);

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
		AcquireSRWLockExclusive(&_sessionEventLocks[i]);
	}

	NetSessionEventQueue* tmp = _pSessionEventsFront;
	_pSessionEventsFront = _pSessionEventsBack;
	_pSessionEventsBack = tmp;

	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		ReleaseSRWLockExclusive(&_sessionEventLocks[i]);
	}

	return _pSessionEventsFront;
}

void NetCore::EndHandleSessionEvents()
{
	_pSessionEventsFront->Flush();
}

NetMessageQueue* NetCore::StartHandleReceivedMessages()
{
	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		AcquireSRWLockExclusive(&_receiveSrwLocks[i]);
	}

	NetMessageQueue* tmp = _pReceiveMessagesFront;
	_pReceiveMessagesFront = _pReceiveMessagesBack;
	_pReceiveMessagesBack = tmp;


	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		ReleaseSRWLockExclusive(&_receiveSrwLocks[i]);
	}

	return _pReceiveMessagesFront;
}

void NetCore::EndHandleReceivedMessages()
{
	_pReceiveMessagesFront->Flush();
}

void NetCore::EndIoThreads()
{
	for (int i = 0; i < NET_CORE_NUM_THREADS; ++i) {
		PostQueuedCompletionStatus(_hIOCP, 0, 0, NULL);
	}
}




unsigned WINAPI NetCore::ThreadIoCompletion(LPVOID pParam)
{
	ThreadArgInfo* arg = (ThreadArgInfo*)pParam;
	unsigned endFlag = 0;

	DWORD               dwTransferredSize = 0;
	DWORD               dwFlag = 0;
	Session*			pSession = NULL;
	LPWSAOVERLAPPED     pOverlapped = NULL;
	BOOL                result = false;

	int threadID = arg->threadID;
	NetCore* pNetCore = arg->pNetCore;

	NetPage* netPage = NULL;
	printf("Start IO Completion thread[%d]..\n", threadID);
	while (TRUE) {
		result = GetQueuedCompletionStatus(pNetCore->_hIOCP, &dwTransferredSize, (PULONG_PTR)&pSession, &pOverlapped, INFINITE);
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
				// SessionManager->OnReceive(dwTransferredSize, sessionID)
				// recieve 완료
				if (dwTransferredSize == 0)
				{
					// 클라이언트의 종료
					// SessionManager->OnReceiveDisconnect()

					
					ESessionRefResult res = pSession->ReduceReference(ESessionRefParam::SESSION_REF_DECREASE_RECV);
					if (res == SESSION_REF_DEACTIVATE) {
						SHORT id = pSession->GetID();
						pNetCore->_sessionManager.RemoveSession(id, COMPLETION_RECV_ERROR);
						pNetCore->WriteSessionEvent(id, ESessionEvent::DELETE_PASSIVE_CLIENT, threadID);
					}
				}
				else
				{
					// 데이터를 수신한 경우
					// SessionManager->OnReceive(dwTransferredSize, sessionID, threadID);

					pSession->OnReceive(dwTransferredSize);
					
					NetMessage* pNetMessage = pSession->GetReceiveNetMessageOrNull();
					AcquireSRWLockExclusive(&pNetCore->_receiveSrwLocks[threadID]);
					while (pNetMessage != NULL) {
						assert(pNetMessage->header.length != 0);
						pNetCore->_pReceiveMessagesBack->WriteNetMessage(pSession->GetID(), pNetMessage);
						pNetMessage = pSession->GetReceiveNetMessageOrNull();
					}
					ReleaseSRWLockExclusive(&pNetCore->_receiveSrwLocks[threadID]);
					
					pSession->RegisterReceive();
				}
			}
			else if (pData->operation == IoOperationData::SEND)
			{
				// sessionManager->OnSendComplete(sessionID);
				pSession->OnSendComplete();
			}
			else if (pData->operation == IoOperationData::ACCEPT)
			{
				AcceptIoOperationData* accpetIoOpData = (AcceptIoOperationData*)pOverlapped;
				pNetCore->OnAccept(accpetIoOpData, threadID);

				
			}
		}
		else
		{
			// 각종 오류 상황
			if (pSession == NULL || pOverlapped == NULL) {
				// 발생하면 안되는 경우
				__debugbreak();
			}

			IoOperationData* pData = CONTAINING_RECORD(pOverlapped, IoOperationData, wol);
			if (pData->operation == IoOperationData::RECEIVE) {
				// Receiving 해제, 
				ESessionRefResult res = pSession->ReduceReference(ESessionRefParam::SESSION_REF_DECREASE_RECV);
				if (res == SESSION_REF_DEACTIVATE) {
					SHORT id = pSession->GetID();
					pNetCore->_sessionManager.RemoveSession(id, COMPLETION_RECV_ERROR);
					pNetCore->WriteSessionEvent(id, ESessionEvent::DELETE_PASSIVE_CLIENT, threadID);
				}
				continue;
			}
			else if (pData->operation == IoOperationData::SEND) {
				// Sending 해제, 
				ESessionRefResult res = pSession->ReduceReference(ESessionRefParam::SESSION_REF_DECREASE_SEND);
				if (res == SESSION_REF_DEACTIVATE) {
					SHORT id = pSession->GetID();
					pNetCore->_sessionManager.RemoveSession(id, COMPLETION_SEND_ERROR);
					pNetCore->WriteSessionEvent(id, ESessionEvent::DELETE_PASSIVE_CLIENT, threadID);
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


void NetCore::OnAccept(AcceptIoOperationData* data, int threadId)
{
	Session* pNewSession = _sessionManager.CreateSession(data->data.socket);

	CreateIoCompletionPort(
		(HANDLE)pNewSession->GetSocket(),
		_hIOCP,
		(ULONG_PTR)pNewSession,
		0
	);

	pNewSession->RegisterReceive();

	WriteSessionEvent(pNewSession->GetID(), ESessionEvent::CREATE_PASSIVE_CLIENT, threadId);

	// Register async accept chain
	PostAccept();
}

bool NetCore::PostAccept()
{
	SOCKET acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (acceptSocket == INVALID_SOCKET) {
		puts("accept socket creation fail");
		return false;
	}

	_acceptIoOperationData.data.socket = acceptSocket;
	ZeroMemory(&_acceptIoOperationData.data.wol, sizeof(OVERLAPPED));
	_acceptIoOperationData.data.operation = IoOperationData::ACCEPT;

	DWORD bytesReceived = 0;
	BOOL result = AcceptEx(
		_hListeningSocket,						// 연결을 수락하는 소켓
		_acceptIoOperationData.data.socket,		// 수락 시 연결되는 소켓
		_acceptIoOperationData.buffer,			// 클라이언트와 서버의 주소를 저장할 버퍼
		0,											// 초기 데이터 길이
		sizeof(SOCKADDR_IN) + 16,				// 로컬 주소를 저장할 버퍼의 길이
		sizeof(SOCKADDR_IN) + 16,				// 원격 주소 정보를 저장할 버퍼 길이
		&bytesReceived,
		&_acceptIoOperationData.data.wol
	);

	if (!result && WSAGetLastError() != ERROR_IO_PENDING) {
		puts("AcceptEx Failed");
		return false;
	}

	return true;
}

void NetCore::InitiateAllocation()
{
	_pReceiveMessagesFront = DNew NetMessageQueue();
	_pReceiveMessagesBack = DNew NetMessageQueue();

	_pSessionEventsFront = DNew NetSessionEventQueue();
	_pSessionEventsBack = DNew NetSessionEventQueue();
}

void NetCore::TerminateAllocation()
{
	delete _pSessionEventsBack;
	delete _pSessionEventsFront;
	delete _pReceiveMessagesFront;
	delete _pReceiveMessagesBack;
}

void NetCore::WriteSessionEvent(UINT32 sessionId, ESessionEvent sessionEvent, int threadId)
{
	AcquireSRWLockExclusive(&_sessionEventLocks[threadId]);
	_pSessionEventsBack->WriteNetSessionEvent(sessionId, sessionEvent);
	ReleaseSRWLockExclusive(&_sessionEventLocks[threadId]);
}

void CreateNetCore(INetCore** dst)
{
	*dst = DNew NetCore;
}

void DeleteNetCore(INetCore* src)
{
	delete src;
}
