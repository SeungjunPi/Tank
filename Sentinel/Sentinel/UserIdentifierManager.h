#pragma once

#include "stdafx.h"
#include "SentinelAliases.h"

// DBIndex, SessionID는 65535까지 허용되며, 중복되는 경우를 상정하지 않음. 사용자가 직접 관리해줘야 함.
class UserIdentifierManager
{
public:
    BOOL AddUserIdentifier(UserDBIndex userDBIndex, SessionID userSessionID);
    BOOL RemoveUserIdentifierByDBIndex(UserDBIndex userDBIndex);
    BOOL RemoveUserIdentifierBySessionID(SessionID userSessionID);

    UserDBIndex GetUserDBIndex(SessionID sessionID);
    SessionID GetUserSessionID(UserDBIndex dbIndex);

private:
    static const UINT MAX_NUM_USER_IDENTIFIER = 65536;

    UserDBIndex _dbIndexes[MAX_NUM_USER_IDENTIFIER] = { 0, };
    SessionID _sessionIDs[MAX_NUM_USER_IDENTIFIER] = { 0, };
};

