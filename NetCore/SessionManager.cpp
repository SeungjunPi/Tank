#include "SessionManager.h"

void SessionManager::Initiate()
{
    _srwLock = SRWLOCK_INIT;
    _pointerTable.Initiate(SHRT_MAX);
    _ids = (SHORT*)malloc(sizeof(SHORT) * SHRT_MAX);
    assert(_ids != nullptr);
    for (SHORT i = 0; i < SHRT_MAX; ++i) {
        _ids[i] = i;
    }
    _currentIdIndex = 0;
}

void SessionManager::Terminate()
{
    for (SHORT id = 0; id < SHRT_MAX; ++id) {
        Session* ptr = (Session*)_pointerTable.Pop(id);
        if (ptr != nullptr) {
            delete ptr;
        }
    }

    bool res = _pointerTable.Terminate();
    if (!res) {
        __debugbreak();
    }
    free(_ids);
    
}

Session* SessionManager::CreateSession(SOCKET hSocket)
{
    AcquireSRWLockExclusive(&_srwLock);
    SHORT idIndex = _currentIdIndex++;
    SHORT id = _ids[idIndex];
    if (id < 0) {
        __debugbreak();
    }
    _ids[idIndex] = -1;
    Session* pSession = DNew Session(hSocket, id);
    bool res = _pointerTable.Insert(id, pSession);
    if (!res) {
        __debugbreak();
    }
    pSession->Initiate();
    ReleaseSRWLockExclusive(&_srwLock);
    return pSession;
}

void SessionManager::RemoveSession(SHORT id, ESessionRemoveReason reason)
{
    AcquireSRWLockExclusive(&_srwLock);
    Session* pSession = (Session*)_pointerTable.Pop(id);
    if (pSession == nullptr) {
        return;
    }
    pSession->Terminate();
    SHORT returnIdIndex = --_currentIdIndex;
    assert(_ids[returnIdIndex] == -1);
    _ids[returnIdIndex] = id;
    delete pSession;
    printf("Remove Session: %d\n", id);
    ReleaseSRWLockExclusive(&_srwLock);
}

void SessionManager::DisconnectAllSessions()
{
    AcquireSRWLockExclusive(&_srwLock);
    UINT32 countSessions = _pointerTable.GetCount();
    UINT32* usingKeys = DNew UINT32[countSessions];
    _pointerTable.GetIdsTo(usingKeys, &countSessions);
    for (SHORT i = 0; i < countSessions; ++i) {
        Session* pSession = (Session*)_pointerTable.Get(usingKeys[i]);
        pSession->Disconnect();
    }

    delete[] usingKeys;

    ReleaseSRWLockExclusive(&_srwLock);
}

void SessionManager::SendMessageTo(UINT32 sessionID, BYTE* msg, UINT32 length)
{
    AcquireSRWLockExclusive(&_srwLock);
    Session* pSession = (Session*)_pointerTable.Get(sessionID);
    if (pSession != nullptr) {
        pSession->Lock();
        pSession->Send(msg, length);
        pSession->Unlock();
    }
    ReleaseSRWLockExclusive(&_srwLock);
}

Session* SessionManager::GetSessionPtr(SHORT id)
{
    AcquireSRWLockExclusive(&_srwLock);
    Session* pSession = (Session*)_pointerTable.Get(id);
    ReleaseSRWLockExclusive(&_srwLock);
    return pSession;
}
