#pragma once

enum class DBErrorCode
{
	NONE,
	SYSTEM_ERROR,
	HAVE_TO_ADD,
};

class IJunDB
{
public:
	virtual DBErrorCode Start(SHORT numThreads) = 0;
	virtual DBErrorCode End() = 0;

	virtual void QueryAsync(SQLWCHAR* pQuery) = 0;

	virtual void ValidatePlayerInfo(WCHAR* ID, WCHAR* pw) = 0;
	virtual void LoadStat(WCHAR* ID) = 0;
	virtual void StoreStat(WCHAR* ID, int hitCount, int killCount, int deathCount) = 0;

};
