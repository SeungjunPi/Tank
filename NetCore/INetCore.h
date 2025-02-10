#pragma once

#define NETCORE_API __declspec(dllexport)
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "NetCoreAliases.h"
#include "NetCorePch.h"
#include "NetCoreCommon.h"

#define NET_CORE_NUM_THREADS (2)
#define NET_CORE_PORT (30283)

class NetMessageQueue;
class NetSessionEventQueue;

class NETCORE_API INetCore
{
public:
	// IOCP 생성
	virtual bool StartNetCore() = 0;

	// Accept 시작
	virtual void StartAccept() =0;

	// IOCP 종료
	virtual void EndNetCore() = 0;

	// 동기 Connection, session ID 반환
	virtual SessionID ConnectTo(const char* ip, int port) = 0;
	virtual void Disconnect(SessionID sessionID) = 0;

	virtual bool SendMessageTo(SessionID sessionID, BYTE* msg, UINT32 length) = 0;
	virtual bool SendMessageTo(SessionID* sessionIDs, UINT32 numSessions, BYTE* msg, UINT32 length) = 0;

	virtual NetSessionEventQueue* StartHandleSessionEvents() = 0;
	virtual void EndHandleSessionEvents() = 0;

	// 싱글 스레드에서 호출돼야 하며, 이후 반드시 End가 호출돼야 함 
	virtual NetMessageQueue* StartHandleReceivedMessages() = 0; 

	// 호출된 시점에 NetMessageQueue의 모든 NetMessage가 처리됐다고 간주함
	virtual void EndHandleReceivedMessages() = 0; 
private:

};


void NETCORE_API CreateNetCore(INetCore** dst);
void NETCORE_API DeleteNetCore(INetCore* src);

