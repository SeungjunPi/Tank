#pragma once

#include "stdafx.h"
#include "Session.h"
#include "PointerTable.h"

enum ESessionRemoveReason
{
	COMPLETION_RECV_ERROR,
	COMPLETION_SEND_ERROR,
	COMPLETION_RECV_DISCONNECT,
	FORCE_REMOVE
};


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
	PointerTable _pointerTable;

	SRWLOCK _srwLock = SRWLOCK_INIT;
};

