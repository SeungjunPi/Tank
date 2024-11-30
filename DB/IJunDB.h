#pragma once

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

class IJunDB
{
public:
	virtual DBErrorCode Start(const DBConnectionInfo connectionInfo, SHORT numThreads) = 0;
	virtual DBErrorCode End() = 0;

	virtual void ValidatePlayerInfo(const WCHAR* ID, const WCHAR* pw) = 0;
	virtual void LoadStat(const WCHAR* ID) = 0;
	virtual void StoreStat(const WCHAR* ID, int hitCount, int killCount, int deathCount) = 0;

};
