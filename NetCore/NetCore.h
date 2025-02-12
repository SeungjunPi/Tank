#pragma once

#include "INetCore.h"
#include "Session.h"
#include "SessionManager.h"

class NetCore;

struct ThreadArgInfo
{
	UINT32 threadID = 0;
	NetCore* pNetCore = nullptr;
	SessionManager* pSessionManager = nullptr;
};

struct AcceptIoOperationData
{
	IoOperationData data;
	CHAR buffer[2 * (sizeof(SOCKADDR_IN) + 16)];
};


class NetCore: public INetCore
{
public:
	bool StartNetCore() override;
	void StartAccept() override;
	void EndNetCore() override;
	UINT32 ConnectTo(const char* ip, int port) override;
	void Disconnect(UINT32 sessionID) override;

	bool SendMessageTo(UINT32 sessionID, BYTE* msg, UINT32 length) override;
	bool SendMessageTo(UINT32* sessionIDs, UINT32 numSessions, BYTE* msg, UINT32 length) override;

	NetSessionEventQueue* StartHandleSessionEvents() override;
	void EndHandleSessionEvents() override;

	NetMessageQueue* StartHandleReceivedMessages() override;
	void EndHandleReceivedMessages() override; 


private:
	static unsigned WINAPI ThreadIoCompletion(LPVOID pParam);

	void OnAccept(AcceptIoOperationData* data, int threadId);
	bool PostAccept();

	void InitiateAllocation();
	void TerminateAllocation();

	void WriteSessionEvent(SessionID sessionId, ESessionEvent sessionEvent, int threadId);

	// 임시
	void WaitNetCore();

	void EndIoThreads();


	WSADATA _wsa = { 0, };

	HANDLE _hIOCP;
	SOCKET _hListeningSocket;
	AcceptIoOperationData _acceptIoOperationData;


	HANDLE _ioThreads[NET_CORE_NUM_THREADS];
	ThreadArgInfo _threadArgs[NET_CORE_NUM_THREADS];

	SRWLOCK _sessionEventLocks[NET_CORE_NUM_THREADS];
	NetSessionEventQueue* _pSessionEventsFront;
	NetSessionEventQueue* _pSessionEventsBack;

	SRWLOCK _receiveSrwLocks[NET_CORE_NUM_THREADS];
	NetMessageQueue* _pReceiveMessagesFront;
	NetMessageQueue* _pReceiveMessagesBack;

	SessionManager _sessionManager;
};

