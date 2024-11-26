#pragma once

#include "pch.h"

const unsigned int PLAYER_ID_MAX_LENGTH = 16;
const unsigned int PASSWORD_MAX_LENGTH = 16;
const unsigned int QUERY_MAX_LENGTH = 1024;

enum class DBEventCode
{
	QUERY_PLAYER_INFO,
	QUERY_LOAD_STAT,
	QUERY_STORE_STAT
};

struct DBEvent
{
	DBEventCode code;
	void* pEvent;
};

struct DBResultPlayerInfo
{
	const WCHAR* ID = nullptr;
	BOOL isSuccess = FALSE;
};

struct DBResultLoadStat
{
	const WCHAR* ID = nullptr;
	int hitCount = 0;
	int killCount = 0;
	int deathCount = 0;
	BOOL isSuccess = FALSE;
};

struct DBResultUpdateStat
{
	const WCHAR* ID = nullptr;
	BOOL isSuccess = FALSE;
};



class DBQueryPlayerInfo
{
public:
	DBQueryPlayerInfo(const WCHAR* ID, const WCHAR* password);
	~DBQueryPlayerInfo();

	WCHAR* GetQuery() const { return _query; }

	const WCHAR* GetPlayerID() const { return _pPlayerID; }

	void SetResult(BOOL result);

	void GetResult(DBResultPlayerInfo* out) const;
private:
	WCHAR* _query;
	WCHAR* _pPlayerID;
	WCHAR* _pPassword;

	BOOL _result = FALSE;
};

class DBQueryLoadStat
{
public:
	DBQueryLoadStat(const WCHAR* ID);
	~DBQueryLoadStat();

	WCHAR* GetQuery() const { return _query; }

	const WCHAR* GetPlayerID() const { return _pPlayerID; }
	void SetResult(int hitCount, int killCount, int deathCount, BOOL isSuccess);
	void GetResult(DBResultLoadStat* out) const;
	
private:
	WCHAR* _query;
	WCHAR* _pPlayerID;
	
	DBResultLoadStat _result;
};

class DBQueryUpdateStat
{
public:
	DBQueryUpdateStat(const WCHAR* ID, int hitCount, int killCount, int deathCount);
	~DBQueryUpdateStat();

	WCHAR* GetQuery() const { return _query; }

	const WCHAR* GetPlayerID() const { return _pPlayerID; }
	void SetResult(BOOL isSuccess);
	void GetResult(DBResultUpdateStat* out) const;
private:
	WCHAR* _query;
	WCHAR* _pPlayerID;
	WCHAR* _pHitCount;
	WCHAR* _pKillCount;
	WCHAR* _pDeathCount;

	DBResultUpdateStat _result;
};



