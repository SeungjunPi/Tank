#pragma once

#include "NetCorePch.h"
#include "Session.h"
#include "NetCoreCommon.h"
#include "LinearQueue.h"
#include "JMap.h"

const int MAX_NUM_SESSIONS = 65536;


const SessionFlag SESSION_FLAG_DEFAULT = 0;
const SessionFlag SESSION_FLAG_ACTIVE = 0b1;
const SessionFlag SESSION_FLAG_RECEIVE_PENDING = 0b10;
const SessionFlag SESSION_FLAG_SEND_PENDING = 0b100;


struct SessionGuard
{
	SRWLOCK lock = SRWLOCK_INIT;
	Session* pSession = nullptr;
	SessionFlag flag = SESSION_FLAG_DEFAULT;
};

class SessionManager
{
public:
	SessionManager() = default;
	~SessionManager() = default;

	void Initiate();
	void Terminate();

	Session* CreateSession(SOCKET hSocket);

	void RemoveSession(SessionID id, ESessionRemoveReason reason);

	void DisconnectAllSessions();

	ENetCoreResult SendMessageTo(SessionID sessionID, BYTE* msg, UINT32 length);

	ENetCoreResult OnSendComplete(SessionID sessionID);

	ENetCoreResult OnFailReceivePending(SessionID sessionID);


	ENetCoreResult HandleErrorOnReceiveComplete(SessionID sessionID);
	ENetCoreResult HandleErrorOnSendComplete(SessionID sessionID);


private:
	SRWLOCK _tableLock = SRWLOCK_INIT;
	SessionGuard _sessionTable[MAX_NUM_SESSIONS];
	LinearQueue _unusedIDs;


};
