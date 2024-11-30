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
	WCHAR ID[PLAYER_ID_MAX_LENGTH + 1];
	BOOL isSuccess = FALSE;
};

struct DBResultLoadStat
{
	WCHAR ID[PLAYER_ID_MAX_LENGTH + 1];
	int hitCount = 0;
	int killCount = 0;
	int deathCount = 0;
	BOOL isSuccess = FALSE;
};

struct DBResultUpdateStat
{
	WCHAR ID[PLAYER_ID_MAX_LENGTH + 1];
	BOOL isSuccess = FALSE;
};

class DBQuery
{
public:
	DBQuery(const WCHAR* ID);
	void GetPlayerID(WCHAR* out) const;
	WCHAR* GetQuery() const { return _query; }

protected:
	WCHAR* _query;
	WCHAR _playerID[PLAYER_ID_MAX_LENGTH + 1];
	CHAR _idLen;
};

class DBQueryPlayerInfo: public DBQuery
{
public:
	DBQueryPlayerInfo(const WCHAR* ID, const WCHAR* password);
	~DBQueryPlayerInfo();

	void SetResult(BOOL isSuccess);
	void GetResult(DBResultPlayerInfo* out) const;
private:
	DBResultPlayerInfo _result;
};

class DBQueryLoadStat: public DBQuery
{
public:
	DBQueryLoadStat(const WCHAR* ID);
	~DBQueryLoadStat();

	void SetResult(int hitCount, int killCount, int deathCount, BOOL isSuccess);
	void GetResult(DBResultLoadStat* out) const;
	
private:
	DBResultLoadStat _result;
};

class DBQueryUpdateStat: public DBQuery
{
public:
	DBQueryUpdateStat(const WCHAR* ID, int hitCount, int killCount, int deathCount);
	~DBQueryUpdateStat();

	void SetResult(BOOL isSuccess);
	void GetResult(DBResultUpdateStat* out) const;
private:
	DBResultUpdateStat _result;
};



