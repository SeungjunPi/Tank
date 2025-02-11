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
    ReleaseSRWLockExclusive(&_sessionTable[newID].lock);

    ReleaseSRWLockExclusive(&_tableLock);
    return pSession;
}

void SessionManager::RemoveSession(SessionID id, ESessionRemoveReason reason)
{
    AcquireSRWLockExclusive(&_tableLock);
    
    Session* pSession = _sessionTable[id].pSession;
    pSession->Terminate();
    delete pSession;
    printf("Remove Session: %d\n", id);
    bool res = _unusedIDs.Push(&id);
    assert(res);

    ReleaseSRWLockExclusive(&_tableLock);
}

void SessionManager::DisconnectAllSessions()
{
    AcquireSRWLockExclusive(&_tableLock);
    
    for (SessionID i = 0; i < MAX_NUM_SESSIONS; ++i) {
        AcquireSRWLockExclusive(&_sessionTable[i].lock);
        Session* pSession = _sessionTable[i].pSession;
        pSession->Disconnect();
        _sessionTable[i].pSession = nullptr;
        ReleaseSRWLockExclusive(&_sessionTable[i].lock);
    }

    ReleaseSRWLockExclusive(&_tableLock);
}

void SessionManager::SendMessageTo(SessionID sessionID, BYTE* msg, UINT32 length)
{
    AcquireSRWLockExclusive(&_sessionTable[sessionID].lock);
    Session* pSession = _sessionTable[sessionID].pSession;
    if (pSession != nullptr) {
        bool isSuccess = pSession->Send(msg, length);
        if (!isSuccess) {
            bool isRemoved = TryRemoveSessionOnSend(sessionID);
            // hmmmmmmmmmmmmmmmmmmm
        }
    }
    ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
}

bool SessionManager::OnSendComplete(SessionID sessionID)
{
    AcquireSRWLockExclusive(&_sessionTable[sessionID].lock);
    Session* pSession = _sessionTable[sessionID].pSession;
    assert(pSession != nullptr);

    pSession->OnSendComplete();

    if (!pSession->_isRecving) {
        pSession->_isSending = false;
        RemoveSession(sessionID, COMPLETION_SEND_ERROR);
        ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
        return false;
    }

    bool isNormalResult = pSession->TryResend();
    if (!isNormalResult) {
        pSession->_isSending = false;
    }
    
    ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
    return true;
}

bool SessionManager::TryRemoveSessionOnReceive(SessionID sessionID)
{
    AcquireSRWLockExclusive(&_sessionTable[sessionID].lock);
    Session* pSession = _sessionTable[sessionID].pSession;
    assert(pSession != nullptr);

    ESessionRefResult res = pSession->ReduceReference(ESessionRefParam::SESSION_REF_DECREASE_RECV);
    if (res == SESSION_REF_DEACTIVATE) {
        SHORT id = pSession->GetID();
        // Send Disconnect Event
        RemoveSession(id, COMPLETION_RECV_ERROR);
    }
    
    ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
    if (res == SESSION_REF_DEACTIVATE) {
        return true;
    }
    return false;
}

bool SessionManager::TryRemoveSessionOnSend(SessionID sessionID)
{
    AcquireSRWLockExclusive(&_sessionTable[sessionID].lock);
    Session* pSession = _sessionTable[sessionID].pSession;
    assert(pSession != nullptr);

    ESessionRefResult res = pSession->ReduceReference(ESessionRefParam::SESSION_REF_DECREASE_SEND);
    if (res == SESSION_REF_DEACTIVATE) {
        SessionID id = pSession->GetID();
        RemoveSession(id, COMPLETION_SEND_ERROR);
    }
    ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);

    if (res == SESSION_REF_DEACTIVATE) {
        return true;
    }
    return false;
}
