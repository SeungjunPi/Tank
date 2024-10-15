#pragma once

#define NETCORE_API __declspec(dllexport)

#include "NetMessageQueue.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define NET_CORE_NUM_THREADS (2)
#define NET_CORE_PORT (30283)

class NETCORE_API NetCore
{
public:
	// IOCP 생성
	bool StartNetCore();

	// Accept 시작, session 생성 시 매개변수로 전달되는 콜백함수 실행
	void StartAccept(void (*sessionCreateCallback)(UINT32 sessionID), void (*sessionDisconnectCallback)(UINT32 sessionID));

	// IOCP 종료
	void EndNetCore();

	// session ID 반환
	UINT32 ConnectTo(const char* ip, int port);
	void Disconnect(UINT32 sessionID);

	// 임시
	void WaitNetCore();


	bool SendMessageTo(UINT32 sessionID, BYTE* msg, UINT32 length);
	bool SendMessageTo(UINT32* sessionIDs, UINT32 numSessions, BYTE* msg, UINT32 length);


	NetMessageQueue* GetReceiveMessages();

	void EndIoThreads();
};


NETCORE_API NetCore* GetNetCore();
NETCORE_API void DeleteNetCore();

NETCORE_API void CreateNetCore(NetCore** dst);
NETCORE_API void DeleteNetCore(NetCore* src);

