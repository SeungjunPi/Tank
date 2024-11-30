#include "DBStructs.h"

DBQuery::DBQuery(const WCHAR* ID)
{
	memset(_playerID, 0, sizeof(WCHAR) * (PLAYER_ID_MAX_LENGTH + 1));
	_idLen = wcsnlen_s(ID, PLAYER_ID_MAX_LENGTH + 1);
	assert(_idLen <= PLAYER_ID_MAX_LENGTH);
	memcpy(_playerID, ID, sizeof(WCHAR) * (_idLen + 1));

	_query = new WCHAR[QUERY_MAX_LENGTH];
	memset(_query, 0, sizeof(WCHAR) * QUERY_MAX_LENGTH);
}

void DBQuery::GetPlayerID(WCHAR* out) const
{
	memcpy(out, &_playerID, sizeof(WCHAR) * (_idLen + 1));
}

DBQueryPlayerInfo::DBQueryPlayerInfo(const WCHAR* ID, const WCHAR* password)
	: DBQuery(ID)
{
	swprintf_s(_query, QUERY_MAX_LENGTH, 
		L"select case when exists (select 1 from tankDB.dbo.Players where PlayerID = '%ls' and Password = '%ls') then 1 else 0 end as existance;", 
		ID, password);
}

DBQueryPlayerInfo::~DBQueryPlayerInfo()
{
	delete[] _query;
}

void DBQueryPlayerInfo::SetResult(BOOL isSuccess)
{
	GetPlayerID(_result.ID);
	_result.isSuccess = isSuccess;
}

void DBQueryPlayerInfo::GetResult(DBResultPlayerInfo* out) const
{
	*out = _result;
}

DBQueryLoadStat::DBQueryLoadStat(const WCHAR* ID)
	: DBQuery(ID)
{
	swprintf_s(_query, QUERY_MAX_LENGTH, 
		L"select * from tankDB.dbo.Scores where PlayerID = '%ls'", 
		ID);
}

DBQueryLoadStat::~DBQueryLoadStat()
{
	delete[] _query;
}

void DBQueryLoadStat::SetResult(int hitCount, int killCount, int deathCount, BOOL isSuccess)
{
	if (!isSuccess) {
		_result.isSuccess = isSuccess;
		return;
	}
	_result.isSuccess = isSuccess;
	GetPlayerID(_result.ID);
	_result.hitCount = hitCount;
	_result.deathCount = deathCount;
	_result.killCount = killCount;
}

void DBQueryLoadStat::GetResult(DBResultLoadStat* out) const
{
	*out = _result;
}

DBQueryUpdateStat::DBQueryUpdateStat(const WCHAR* ID, int hitCount, int killCount, int deathCount)
	: DBQuery(ID)
{
	swprintf_s(_query, QUERY_MAX_LENGTH,
		L"update tankDB.dbo.Scores set Hit = %i, KillCount = %i, Death = %i where PlayerID = '%ls';", 
		hitCount, killCount, deathCount, ID);
}

DBQueryUpdateStat::~DBQueryUpdateStat()
{
	delete[] _query;
}

void DBQueryUpdateStat::SetResult(BOOL isSuccess)
{
	GetPlayerID(_result.ID);
	_result.isSuccess = isSuccess;
}

void DBQueryUpdateStat::GetResult(DBResultUpdateStat* out) const
{
	*out = _result;
}
