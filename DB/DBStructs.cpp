#include "DBStructs.h"

DBQueryPlayerInfo::DBQueryPlayerInfo(const WCHAR* ID, const WCHAR* password)
{
	size_t idLength = wcsnlen_s(ID, PLAYER_ID_MAX_LENGTH + 1);
	size_t pwLength = wcsnlen_s(password, PASSWORD_MAX_LENGTH + 1);

	assert(idLength <= PLAYER_ID_MAX_LENGTH);
	assert(pwLength <= PLAYER_ID_MAX_LENGTH);

	_query = new WCHAR[QUERY_MAX_LENGTH];
	memset(_query, 0, sizeof(WCHAR) * QUERY_MAX_LENGTH);

	
	swprintf_s(_query, QUERY_MAX_LENGTH, 
		L"select case when exists (select 1 from tankDB.dbo.Players where PlayerID = '%ls' and Password = '%ls') then 1 else 0 end as existance;", 
		ID, password);
}

DBQueryPlayerInfo::~DBQueryPlayerInfo()
{
	delete[] _query;
}

void DBQueryPlayerInfo::SetResult(BOOL result)
{
	_result = result;
}

void DBQueryPlayerInfo::GetResult(DBResultPlayerInfo* out) const
{
	if (_result) {
		out->ID = _pPlayerID;
		out->isSuccess = _result;
	}
}

DBQueryLoadStat::DBQueryLoadStat(const WCHAR* ID)
{
	size_t idLength = wcsnlen_s(ID, PLAYER_ID_MAX_LENGTH + 1);
	assert(idLength <= PLAYER_ID_MAX_LENGTH);

	_query = new WCHAR[QUERY_MAX_LENGTH];
	memset(_query, 0, sizeof(WCHAR) * QUERY_MAX_LENGTH);
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
	_result.ID = _pPlayerID;
	_result.hitCount = hitCount;
	_result.deathCount = deathCount;
	_result.killCount = killCount;
}

void DBQueryLoadStat::GetResult(DBResultLoadStat* out) const
{
	*out = _result;
}

DBQueryUpdateStat::DBQueryUpdateStat(const WCHAR* ID, int hitCount, int killCount, int deathCount)
{
	size_t idLength = wcsnlen_s(ID, PLAYER_ID_MAX_LENGTH + 1);
	assert(idLength <= PLAYER_ID_MAX_LENGTH);

	_query = new WCHAR[QUERY_MAX_LENGTH];
	memset(_query, 0, sizeof(WCHAR) * QUERY_MAX_LENGTH);

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
	_result.ID = _pPlayerID;
	_result.isSuccess = isSuccess;
}

void DBQueryUpdateStat::GetResult(DBResultUpdateStat* out) const
{
	*out = _result;
}
