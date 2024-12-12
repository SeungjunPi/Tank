#pragma once

#include "stdafx.h"
#include <string>

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

	virtual void ValidatePlayerInfo(const WCHAR* ID, const WCHAR* pw) = 0;
	virtual void LoadStat(const WCHAR* ID) = 0;
	virtual void UpdateStat(const WCHAR* ID, int hitCount, int killCount, int deathCount) = 0;

	// 이 함수를 통해 얻은 큐에서 꺼낸 DBEvent는 처리 후 직접 소멸자를 호출해줘야 함.
	virtual CQueue<DBEvent>* BeginHandleResult() = 0;
	virtual void EndHandleResult() = 0;
};
