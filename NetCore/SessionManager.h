#pragma once

#include "NetCorePch.h"
#include "Session.h"
#include "NetCoreCommon.h"
#include "LinearQueue.h"
#include "JMap.h"

const int MAX_NUM_SESSIONS = 65536;

struct SessionGuard
{
	SRWLOCK lock = SRWLOCK_INIT;
	Session* pSession = nullptr;
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

	void SendMessageTo(SessionID sessionID, BYTE* msg, UINT32 length);

	// Resend or ..
	bool OnSendComplete(SessionID sessionID);
	bool TryRemoveSessionOnReceive(SessionID sessionID);
	bool TryRemoveSessionOnSend(SessionID sessionID);


private:
	SRWLOCK _tableLock = SRWLOCK_INIT;
	SessionGuard _sessionTable[MAX_NUM_SESSIONS];
	LinearQueue _unusedIDs;
};
