#include "DBStruct.h"

DBQuery::DBQuery(int userID)
	: _userID(userID)
{
}

DBQueryValidation::DBQueryValidation(const WCHAR* ID, const WCHAR* password, UINT32 sessionID)
	: DBQuery(0)
	, _sessionID(sessionID)
{
	WCHAR* strBuffer = _query.GetWStringBuffer();
	size_t writeLength = swprintf_s(strBuffer, LiteWString::MAX_WSTR_LENGTH,
		L"select UserID from tankDB.dbo.Users where UserName='%ls' and Password='%ls';",
		ID, password);
	_userName.Clear();
	_userName.Append(ID);
	assert(writeLength < SHRT_MAX);
	_query.SetLength((short)writeLength);
	_result.code = DBResultCode::FAIL_CONNECTION_FAIL;
}

DBQueryValidation::~DBQueryValidation()
{
}

void DBQueryValidation::SetResult(DBResultCode code, int userID)
{
	_result.code = code;
	_userID = userID;
}

void DBQueryValidation::GetResult(DBResultUserValidation* out) const
{
	*out = _result;
}


DBQueryLoadStat::DBQueryLoadStat(int userID, UINT32 sessionID)
	: DBQuery(userID)
	, _sessionID(sessionID)
{
	WCHAR* strBuffer = _query.GetWStringBuffer();
	size_t writeLength = swprintf_s(strBuffer, LiteWString::MAX_WSTR_LENGTH,
		L"select HitCount, KillCount, DeathCount from tankDB.dbo.TankScores where UserID=%d;",
		userID);
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

void DBQueryLoadStat::GetResult(DBResultLoadScore* out) const
{
	*out = _result;
}

DBQueryUpdateStat::DBQueryUpdateStat(int userID, int hitCount, int killCount, int deathCount)
	: DBQuery(userID)
{
	WCHAR* strBuffer = _query.GetWStringBuffer();
	size_t writeLength = swprintf_s(strBuffer, LiteWString::MAX_WSTR_LENGTH,
		L"update tankDB.dbo.TankScores set HitCount=%d, KillCount=%d, DeathCount=%d where UserID=%d;",
		hitCount, killCount, deathCount, userID);
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

