#pragma once

#include "NetCorePch.h"
#include "Session.h"
#include "PointerTable.h"
#include "NetCoreCommon.h"

class SessionManager
{
public:
	SessionManager() = default;
	~SessionManager() = default;

	void Initiate();
	void Terminate();

	Session* CreateSession(SOCKET hSocket);

	void RemoveSession(SHORT id, ESessionRemoveReason reason);

	void DisconnectAllSessions();

	void SendMessageTo(UINT32 sessionID, BYTE* msg, UINT32 length);

	Session* GetSessionPtr(SHORT id);

private:
	SHORT* _ids = nullptr;
	SHORT _currentIdIndex = 0;
	PointerTable32 _pointerTable;

	SRWLOCK _srwLock = SRWLOCK_INIT;
};

