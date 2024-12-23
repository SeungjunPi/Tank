#include "UserIdentifierManager.h"

BOOL UserIdentifierManager::AddUserIdentifier(UserDBIndex userDBIndex, SessionID userSessionID)
{
	if (_dbIndexes[userSessionID] != 0 || _sessionIDs[userDBIndex] != 0) {
		// Exceed 
		__debugbreak();
	}
	_dbIndexes[userSessionID] = userDBIndex;
	_sessionIDs[userDBIndex] = userSessionID;
	return 0;
}

BOOL UserIdentifierManager::RemoveUserIdentifierByDBIndex(UserDBIndex userDBIndex)
{
	SessionID sessionID = _sessionIDs[userDBIndex];
	if (sessionID == INVALID_SESSION_ID) {
		__debugbreak();
		return false;
	}

	UserDBIndex dbIndex = _dbIndexes[sessionID];
	if (dbIndex == 0) {
		__debugbreak();
		return false;
	}

	_sessionIDs[dbIndex] = 0;
	_dbIndexes[sessionID] = 0;

	return true;
}

BOOL UserIdentifierManager::RemoveUserIdentifierBySessionID(SessionID userSessionID)
{
	UserDBIndex dbIndex = _dbIndexes[userSessionID];
	if (dbIndex == 0) {
		__debugbreak();
		return false;
	}

	SessionID sessionID = _sessionIDs[dbIndex];
	if (sessionID == INVALID_SESSION_ID) {
		__debugbreak();
		return false;
	}

	_sessionIDs[dbIndex] = 0;
	_dbIndexes[sessionID] = 0;

	return true;
}

UserDBIndex UserIdentifierManager::GetUserDBIndex(SessionID sessionID)
{
	UserDBIndex dbIndex = _dbIndexes[sessionID];
	return dbIndex;
}

SessionID UserIdentifierManager::GetUserSessionID(UserDBIndex dbIndex)
{
	SessionID sessionID = _sessionIDs[dbIndex];
	if (sessionID == INVALID_SESSION_ID) {
		__debugbreak();
	}
	return sessionID;
}



