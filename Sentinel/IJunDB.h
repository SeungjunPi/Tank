#pragma once

#include <string>
#include "SentinelPch.h"


enum class DBErrorCode
{
	NONE,
	SYSTEM_ERROR,
	HAVE_TO_ADD,
};

struct DBConnectionInfo
{
	std::string ip;
	std::string port;
	std::string dbName;
	std::string uid;
	std::string pwd;
};

struct DBEvent;

class IJunDB
{
public:
	virtual DBErrorCode Start(const DBConnectionInfo connectionInfo, SHORT numThreads) = 0;
	virtual DBErrorCode End() = 0;

	virtual void ValidateUserInfo(const WCHAR* ID, const WCHAR* pw, UINT32 sessionID) = 0;
	virtual void LoadStat(UINT32 sessionID, int userID) = 0;
	virtual void UpdateStat(int userID, int hitCount, int killCount, int deathCount) = 0;

	// 이 함수를 통해 얻은 큐에서 꺼낸 DBEvent는 처리 후 직접 소멸자를 호출해줘야 함.
	virtual void BeginHandleResult() = 0;
	virtual BOOL TryGetEvent(DBEvent* out) = 0;
	virtual void EndHandleResult() = 0;
};
