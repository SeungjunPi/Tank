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

    AcquireSRWLockExclusive(&_sessionTable[id].lock);
    
    Session* pSession = _sessionTable[id].pSession;
    pSession->Terminate();
    delete pSession;
    printf("Remove Session: %d\n", id);
    bool res = _unusedIDs.Push(&id);
    assert(res);

    ReleaseSRWLockExclusive(&_sessionTable[id].lock);

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
        pSession->Send(msg, length);
    }
    ReleaseSRWLockExclusive(&_sessionTable[sessionID].lock);
}
