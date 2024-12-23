#pragma once


#include "pch.h"
#include "LiteWString.h"

const unsigned int USER_NAME_MAX_LENGTH = 16;
const unsigned int PASSWORD_MAX_LENGTH = 16;

enum class DBEventCode
{
	QUERY_VALIDATION,
	QUERY_LOAD_STAT,
	QUERY_UPDATE_STAT,
};

enum class DBResultCode
{
	SUCCESS,
	FAIL_INVALID_USER_INFO,
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

struct DBResultUserValidation
{
	int userID = 0; 
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
	DBQuery(int userID);
	int GetID() const { return _userID; }
	const WCHAR* GetQuery() const { return _query.GetWString(); }
protected:
	int _userID;
	LiteWString _query;
};

class DBQueryValidation: public DBQuery
{
public:
	DBQueryValidation(const WCHAR* name, const WCHAR* password);
	~DBQueryValidation();

	const WCHAR* GetName() const { return _userName.GetWString(); }

	void SetResult(DBResultCode code, int userID);
	void GetResult(DBResultUserValidation* out) const;
private:
	DBResultUserValidation _result;
	LiteWString _userName;
};

class DBQueryLoadStat: public DBQuery
{
public:
	DBQueryLoadStat(int userID);
	~DBQueryLoadStat();

	void SetResult(DBResultCode code, int hitCount, int killCount, int deathCount);
	void GetResult(DBResultLoadStat* out) const;
	
private:
	DBResultLoadStat _result;
};

class DBQueryUpdateStat: public DBQuery
{
public:
	DBQueryUpdateStat(int userID, int hitCount, int killCount, int deathCount);
	~DBQueryUpdateStat();

	void SetResult(DBResultCode code);
	void GetResult(DBResultUpdateStat* out) const;
private:
	DBResultUpdateStat _result;
};



