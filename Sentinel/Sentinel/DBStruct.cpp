#include "DBStruct.h"

DBQuery::DBQuery(const WCHAR* ID)
	: _playerID(ID)
{
}

const WCHAR* DBQuery::GetPlayerID() const
{
	return _playerID.GetWString();
}

DBQueryPlayerInfo::DBQueryPlayerInfo(const WCHAR* ID, const WCHAR* password)
	: DBQuery(ID)
{
	WCHAR* strBuffer = _query.GetWStringBuffer();
	size_t writeLength = swprintf_s(strBuffer, LiteWString::MAX_WSTR_LENGTH,
		L"select case when exists (select 1 from tankDB.dbo.Players where PlayerID = '%ls' and Password = '%ls') then 1 else 0 end as existance;",
		ID, password);
	assert(writeLength < SHRT_MAX);
	_query.SetLength((short)writeLength);
	_result.code = DBResultCode::FAIL_CONNECTION_FAIL;
}

DBQueryPlayerInfo::~DBQueryPlayerInfo()
{
}

void DBQueryPlayerInfo::SetResult(DBResultCode code)
{
	_result.code = code;
}

void DBQueryPlayerInfo::GetResult(DBResultPlayerInfo* out) const
{
	*out = _result;
}

DBQueryLoadStat::DBQueryLoadStat(const WCHAR* ID)
	: DBQuery(ID)
{
	WCHAR* strBuffer = _query.GetWStringBuffer();
	size_t writeLength = swprintf_s(strBuffer, LiteWString::MAX_WSTR_LENGTH,
		L"select * from tankDB.dbo.Scores where PlayerID = '%ls'",
		ID);
	assert(writeLength < SHRT_MAX);
	_query.SetLength((short)writeLength);
	_result.code = DBResultCode::FAIL_CONNECTION_FAIL;
}

DBQueryLoadStat::~DBQueryLoadStat()
{
}

void DBQueryLoadStat::SetResult(DBResultCode code, int hitCount, int killCount, int deathCount)
{
	_result.code = code;
	if (code != DBResultCode::SUCCESS) {
		return;
	}

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
	WCHAR* strBuffer = _query.GetWStringBuffer();
	size_t writeLength = swprintf_s(strBuffer, LiteWString::MAX_WSTR_LENGTH,
		L"update tankDB.dbo.Scores set Hit = %i, KillCount = %i, Death = %i where PlayerID = '%ls';",
		hitCount, killCount, deathCount, ID);
	assert(writeLength < SHRT_MAX);
	_query.SetLength((short)writeLength);
	_result.code = DBResultCode::FAIL_CONNECTION_FAIL;
}

DBQueryUpdateStat::~DBQueryUpdateStat()
{

}

void DBQueryUpdateStat::SetResult(DBResultCode code)
{
	_result.code = code;
}

void DBQueryUpdateStat::GetResult(DBResultUpdateStat* out) const
{
	*out = _result;
}
