#include "SessionManager.h"

void SessionManager::Initiate()
{
    _unusedIDs.Initiate(sizeof(SessionID), MAX_NUM_SESSIONS);
    for (SessionID id = 0; id < MAX_NUM_SESSIONS; ++id) {
        bool res = _unusedIDs.Push(&id);
        assert(res);
    }
}

void SessionManager::Terminate()
{
    for (SessionID id = 0; id < MAX_NUM_SESSIONS; ++id) {
        AcquireSRWLockExclusive(&_sessionTable[id].lock);
        // 이미 Disconnect로 인해 정리됐어야 함.
        assert(_sessionTable[id].pSession == nullptr);
        ReleaseSRWLockExclusive(&_sessionTable[id].lock);
    }
    
    _unusedIDs.Terminate();
}

Session* SessionManager::CreateSession(SOCKET hSocket)
{
    AcquireSRWLockExclusive(&_tableLock);
    
    int flag = 1;
    setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));

    SessionID newID;
    _unusedIDs.TryPopTo(&newID);
    assert(newID != INVALID_SESSION_ID);

    AcquireSRWLockExclusive(&_sessionTable[newID].lock);
    Session* pSession = DNew Session(hSocket, newID);
    pSession->Initiate();
    _sessionTable[newID].pSession = pSession;
    _sessionTable[newID].flag = SESSION_FLAG_ACTIVE;
    ReleaseSRWLockExclusive(&_sessionTable[newID].lock);
    ReleaseSRWLockExclusive(&_tableLock);
    return pSession;
}

void SessionManager::RemoveSession(SessionID id, ESessionRemoveReason reason)
{
    AcquireSRWLockExclusive(&_tableLock);

	assert(_sessionTable[id].flag == SESSION_FLAG_DEFAULT);
    
    Session* pSession = _sessionTable[id].pSession;
	_sessionTable[id].pSession = nullptr;
    pSession->Terminate();
    delete pSession;
    printf("Remove Session: %d\n", id);
    bool res = _unusedIDs.Push(&id);
    assert(res);

    ReleaseSRWLockExclusive(&_tableLock);
}

void SessionManager::DisconnectAllSessions()
{
	for (SessionID i = 0; i < MAX_NUM_SESSIONS; ++i) {
		AcquireSRWLockExclusive(&_sessionTable[i].lock);
		Session* pSession = _sessionTable[i].pSession;
		if (pSession != nullptr) {
			_sessionTable[i].flag &= ~SESSION_FLAG_ACTIVE;
			pSession->CloseSocket();
		}
		ReleaseSRWLockExclusive(&_sessionTable[i].lock);
	}

	/*for (SessionID i = 0; i < MAX_NUM_SESSIONS; ++i) {
		AcquireSRWLockExclusive(&_sessionTable[i].lock);
	}
    AcquireSRWLockExclusive(&_tableLock);

	for (SessionID i = 0; i < MAX_NUM_SESSIONS; ++i) {
		Session* pSession = _sessionTable[i].pSession;
		
		_sessionTable[i].pSession = nullptr;
	}

	ReleaseSRWLockExclusive(&_tableLock);

	for (SessionID i = 0; i < MAX_NUM_SESSIONS; ++i) {
	    ReleaseSRWLockExclusive(&_sessionTable[i].lock);
	}*/
}

ENetCoreResult SessionManager::BeginReceive(SessionID id)
{
	ENetCoreResult result;
	AcquireSRWLockExclusive(&_sessionTable[id].lock);

	if (_sessionTable[id].flag & SESSION_FLAG_ACTIVE) {
		Session* pSession = _sessionTable[id].pSession;
		bool isSuccessed = pSession->RegisterReceive();
		if (isSuccessed) {
			_sessionTable[id].flag |= SESSION_FLAG_RECEIVE_PENDING;
			result = NC_SUCCESS;
		}
		else {
			result = NC_ERROR_REQUEST_FAIL;
		}
	}
	else {
		result = NC_ERROR_REQUEST_FAIL;
	}

	ReleaseSRWLockExclusive(&_sessionTable[id].lock);
	return result;
}

ENetCoreResult SessionManager::SendMessageTo(SessionID sessionID, BYTE* msg, UINT32 length)
{
	AcquireSRWLockExclusive(&_sessionTable[sessionID].lock);
	Session* pSession = _sessionTable[sessionID].pSession;
	SessionFlag flag = _sessionTable[sessionID].flag;
	bool isSendSuccessed = false;
	if (!(flag & SESSION_FLAG_ACTIVE)) {
		// NetCore 내부 쓰레드에서 이 상황이 발생한 경우, 심각한 오류.
		// 제거 중인 상황이지만, 외부에서 Event를 반영하기 전에 호출된 경우. 
		ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
		return NC_ERROR_REQUEST_FAIL;
	}

	assert(pSession != nullptr);
	
	pSession->WriteSendMessage(msg, length);
	if (!(flag & SESSION_FLAG_SEND_PENDING)) {
		// 아직 send에 대해 pending중이 아닌 경우
		isSendSuccessed = pSession->RegisterSend();
		if (isSendSuccessed) {
			_sessionTable[sessionID].flag |= SESSION_FLAG_SEND_PENDING;
		}
		else {
			// WSASend 실패
			ENetCoreResult result;
			_sessionTable[sessionID].flag &= ~SESSION_FLAG_ACTIVE;
			if (flag & SESSION_FLAG_RECEIVE_PENDING) {
				pSession->CloseSocket();
				result = NC_ERROR_REQUEST_FAIL;
			}
			else {
				// 마지막으로 처리중인 IO
				RemoveSession(sessionID, COMPLETION_SEND_ERROR);
				result = NC_ERROR_SESSION_REMOVED;
			}
			ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
			return result;
		}
	}
	ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
	return NC_SUCCESS;
	
	
}

ENetCoreResult SessionManager::OnSendComplete(SessionID sessionID)
{
    AcquireSRWLockExclusive(&_sessionTable[sessionID].lock);
    Session* pSession = _sessionTable[sessionID].pSession;
	assert(pSession != nullptr);
	SessionFlag flag = _sessionTable[sessionID].flag;
    
	assert(flag & SESSION_FLAG_SEND_PENDING);

	_sessionTable[sessionID].flag &= ~SESSION_FLAG_SEND_PENDING;
    pSession->ResetSendFrontPage();
	if (!(flag & SESSION_FLAG_ACTIVE)) {
		ENetCoreResult result;
		if (!(flag & SESSION_FLAG_RECEIVE_PENDING)) {
			RemoveSession(sessionID, COMPLETION_SEND_ERROR);
			result = NC_ERROR_SESSION_REMOVED;
		}
		else {
			result = NC_ERROR_REQUEST_FAIL;
		}
		ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
		return result;
	}

	bool isReSendNeeded = pSession->TrySwapSendPages();
	if (!isReSendNeeded) {
		ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
		return NC_SUCCESS;
	}

	bool isSendSuccessed = pSession->RegisterSend();
	if (!isSendSuccessed) {
		// WSASend 실패
		_sessionTable[sessionID].flag &= ~SESSION_FLAG_ACTIVE;
		ENetCoreResult result;

		if (flag & SESSION_FLAG_RECEIVE_PENDING) {
			pSession->CloseSocket();
			result = NC_ERROR_REQUEST_FAIL;
		}
		else {
			// 마지막으로 처리중인 IO
			RemoveSession(sessionID, COMPLETION_SEND_ERROR);
			result = NC_ERROR_SESSION_REMOVED;
		}

		ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
		return result;
	}

	_sessionTable[sessionID].flag |= SESSION_FLAG_SEND_PENDING;
	ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
    return NC_SUCCESS;
}

ENetCoreResult SessionManager::OnFailReceivePending(SessionID sessionID)
{
	AcquireSRWLockExclusive(&_sessionTable[sessionID].lock);
	Session* pSession = _sessionTable[sessionID].pSession;
	assert(pSession != nullptr);
	
	SessionFlag flag = _sessionTable[sessionID].flag;
	assert(flag & SESSION_FLAG_RECEIVE_PENDING);
	
	_sessionTable[sessionID].flag &= ~SESSION_FLAG_RECEIVE_PENDING;	
	_sessionTable[sessionID].flag &= ~SESSION_FLAG_ACTIVE;

	ENetCoreResult result;
	if (flag & SESSION_FLAG_SEND_PENDING) {
		pSession->CloseSocket();
		result = NC_ERROR_REQUEST_FAIL;
	}
	else {
		RemoveSession(sessionID, COMPLETION_RECV_ERROR);
		result = NC_ERROR_SESSION_REMOVED;
	}
	ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
	return result;
}

ENetCoreResult SessionManager::HandleErrorOnReceiveComplete(SessionID sessionID)
{
    AcquireSRWLockExclusive(&_sessionTable[sessionID].lock);
    Session* pSession = _sessionTable[sessionID].pSession;
    assert(pSession != nullptr);

	assert(_sessionTable[sessionID].flag & SESSION_FLAG_RECEIVE_PENDING);
	
	_sessionTable[sessionID].flag &= ~SESSION_FLAG_RECEIVE_PENDING;
	_sessionTable[sessionID].flag &= ~SESSION_FLAG_ACTIVE;
	SessionFlag flag = _sessionTable[sessionID].flag;
	
	ENetCoreResult result;
    if (flag & SESSION_FLAG_SEND_PENDING) {
		pSession->CloseSocket();
		result = NC_NONE;
	}
	else {
		RemoveSession(sessionID, COMPLETION_RECV_ERROR);
		result = NC_ERROR_SESSION_REMOVED;
	}
    
    ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
    return result;
}

ENetCoreResult SessionManager::HandleErrorOnSendComplete(SessionID sessionID)
{
    AcquireSRWLockExclusive(&_sessionTable[sessionID].lock);
    Session* pSession = _sessionTable[sessionID].pSession;
    assert(pSession != nullptr);

	assert(_sessionTable[sessionID].flag & SESSION_FLAG_SEND_PENDING);
	

	_sessionTable[sessionID].flag &= ~SESSION_FLAG_SEND_PENDING;
	_sessionTable[sessionID].flag &= ~SESSION_FLAG_ACTIVE;
	SessionFlag flag = _sessionTable[sessionID].flag;

	ENetCoreResult result;

	if (flag & SESSION_FLAG_RECEIVE_PENDING) {
		pSession->CloseSocket();
		result = NC_NONE;
	}
	else {
		RemoveSession(sessionID, COMPLETION_SEND_ERROR);
		result = NC_ERROR_SESSION_REMOVED;
	}

    ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
	return result;
}
