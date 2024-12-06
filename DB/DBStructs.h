#pragma once


#include "pch.h"
#include "LiteWString.h"

const unsigned int PLAYER_ID_MAX_LENGTH = 16;
const unsigned int PASSWORD_MAX_LENGTH = 16;

enum class DBEventCode
{
	QUERY_PLAYER_INFO,
	QUERY_LOAD_STAT,
	QUERY_UPDATE_STAT,
};

enum class DBResultCode
{
	SUCCESS,
	FAIL_INVALID_PLAYER_INFO,
	FAIL_VALIDATION_TYPE_MISMATCH,
	FAIL_CONNECTION_FAIL,
	FAIL_UPDATE_STAT_NO_DATA_WITH_PLAYER_ID,
	FAIL_UPDATE_STAT_WRONG_AFFECTED_COUNT
};

struct DBEvent
{
	DBEventCode code;
	void* pEvent;
};

struct DBResultPlayerInfo
{
	DBResultCode code;
};

struct DBResultLoadStat
{
	int hitCount = 0;
	int killCount = 0;
	int deathCount = 0;
	DBResultCode code;
};

struct DBResultUpdateStat
{
	DBResultCode code;
};

class DBQuery
{
public:
	DBQuery(const WCHAR* ID);
	const WCHAR* GetPlayerID() const;
	const WCHAR* GetQuery() const { return _query.GetWString(); }

protected:
	LiteWString _query;
	LiteWString _playerID;
};

class DBQueryPlayerInfo: public DBQuery
{
public:
	DBQueryPlayerInfo(const WCHAR* ID, const WCHAR* password);
	~DBQueryPlayerInfo();

	void SetResult(DBResultCode code);
	void GetResult(DBResultPlayerInfo* out) const;
private:
	DBResultPlayerInfo _result;
};

class DBQueryLoadStat: public DBQuery
{
public:
	DBQueryLoadStat(const WCHAR* ID);
	~DBQueryLoadStat();

	void SetResult(DBResultCode code, int hitCount, int killCount, int deathCount);
	void GetResult(DBResultLoadStat* out) const;
	
private:
	DBResultLoadStat _result;
};

class DBQueryUpdateStat: public DBQuery
{
public:
	DBQueryUpdateStat(const WCHAR* ID, int hitCount, int killCount, int deathCount);
	~DBQueryUpdateStat();

	void SetResult(DBResultCode code);
	void GetResult(DBResultUpdateStat* out) const;
private:
	DBResultUpdateStat _result;
};



