#include <atlconv.h>

#include "pch.h"
#include "JunDB.h"



static LiteWString s_dbConnectionInfo;

static HANDLE* s_hQueryEvents = nullptr;
static HANDLE s_hEndEvent;

static CQueue<DBEvent> s_queryCQueue;
static CQueue<DBEvent>* s_pResultCQueueFront;
static CQueue<DBEvent>* s_pResultCQueueBack;

static SRWLOCK s_resultQueueLock;
static BOOL s_isResultQueueProcessing;

unsigned WINAPI ThreadDB(LPVOID pParam);

void HandleQueryPlayerInfo(HANDLE hStmt, DBEvent dbEvent);
void HandleQueryLoadPlayerStat(HANDLE hStmt, DBEvent dbEvent);
void HandleQueryUpdatePlayerStat(HANDLE hStmt, DBEvent dbEvent);

void PushResult(DBEvent dbEvent);



void CreateJunDB(IJunDB** out_pJunDB)
{
	*out_pJunDB = new JunDB;
}

void TerminateJunDB(IJunDB* pJunDB)
{
	delete pJunDB;
}

DBErrorCode JunDB::Start(const DBConnectionInfo connectionInfo, SHORT numThreads)
{
	_numThreads = numThreads;
	char buffer[LiteWString::MAX_WSTR_LENGTH + 1];
		
	size_t length = sprintf_s(buffer, LiteWString::MAX_WSTR_LENGTH, 
		"DRIVER={SQL Server};SERVER=%s, %s; DATABASE=%s; UID=%s; PWD=%s;",
		connectionInfo.ip.c_str(),
		connectionInfo.port.c_str(),
		connectionInfo.dbName.c_str(),
		connectionInfo.uid.c_str(),
		connectionInfo.pwd.c_str());

	USES_CONVERSION;
	s_dbConnectionInfo.Append(A2W(buffer));

	s_pResultCQueueFront = new CQueue<DBEvent>;
	s_pResultCQueueBack = new CQueue<DBEvent>;

	s_hThreads = new HANDLE[numThreads];
	s_hQueryEvents = new HANDLE[numThreads];
		
	s_hEndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	s_resultQueueLock = SRWLOCK_INIT;
	s_isResultQueueProcessing = false;

	for (int i = 0; i < numThreads; ++i) {
		s_hQueryEvents[i] = CreateEvent(NULL, FALSE, FALSE, TEXT("QUERY"));
		s_hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadDB, &s_hQueryEvents, 0, NULL);
	}
	
	return DBErrorCode::NONE;
}

DBErrorCode JunDB::End()
{
	SetEvent(s_hEndEvent);
	DWORD result = WaitForMultipleObjects(_numThreads, s_hThreads, TRUE, 10000); // wait 10s
	for (DWORD i = 0; i < _numThreads; ++i) {
		CloseHandle(s_hThreads[i]);
	}
	CloseHandle(s_hEndEvent);
	if (result == WAIT_OBJECT_0) {
		delete[] s_hThreads;
		return DBErrorCode::NONE;
	}

	// TODO: Queue 비우기
	delete s_pResultCQueueFront;
	delete s_pResultCQueueBack;

	return DBErrorCode::NONE;
}

void JunDB::ValidateUserInfo(const WCHAR* ID, const WCHAR* pw)
{
	DBEvent ev;
	ev.code = DBEventCode::QUERY_VALIDATION;

	DBQueryValidation* query = new DBQueryValidation(ID, pw);
	ev.pEvent = (void*)query;

	s_queryCQueue.Push(ev);
	SetQueryEvents();
}

void JunDB::LoadStat(int userID)
{
	DBEvent ev;
	ev.code = DBEventCode::QUERY_LOAD_STAT;

	DBQueryLoadStat* query = new DBQueryLoadStat(userID);

	ev.pEvent = (void*)query;

	s_queryCQueue.Push(ev);
	SetQueryEvents();
}

void JunDB::UpdateStat(int userID, int hitCount, int killCount, int deathCount)
{
	DBEvent ev;
	ev.code = DBEventCode::QUERY_UPDATE_STAT;

	DBQueryUpdateStat* query = new DBQueryUpdateStat(userID, hitCount, killCount, deathCount);
	ev.pEvent = (void*)query;

	s_queryCQueue.Push(ev);
	SetQueryEvents();
}

CQueue<DBEvent>* JunDB::BeginHandleResult()
{
	AcquireSRWLockExclusive(&s_resultQueueLock);
	if (s_isResultQueueProcessing) {
		__debugbreak();
	}

	s_isResultQueueProcessing = true;
	auto tmp = s_pResultCQueueBack;
	s_pResultCQueueBack = s_pResultCQueueFront;
	s_pResultCQueueFront = tmp;
	ReleaseSRWLockExclusive(&s_resultQueueLock);

	return s_pResultCQueueFront;
}

void JunDB::EndHandleResult()
{
	AcquireSRWLockExclusive(&s_resultQueueLock);
	if (!s_isResultQueueProcessing) {
		__debugbreak();
	}
	s_isResultQueueProcessing = false;
	ReleaseSRWLockExclusive(&s_resultQueueLock);
}

void JunDB::SetQueryEvents() const
{
	for (DWORD i = 0; i < _numThreads; ++i) {
		SetEvent(s_hQueryEvents[i]);
	}
}

unsigned WINAPI ThreadDB(LPVOID pParam)
{
	static LONG s_threadID = 0;
	LONG threadID = InterlockedIncrement(&s_threadID);
	--threadID;

	SQLHENV hEnv = nullptr;
	SQLHDBC hDbc = nullptr;
	SQLHSTMT hStmt = nullptr;
	SQLRETURN retCode;

	retCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	if (!SQL_SUCCEEDED(retCode)) {
		return (unsigned)DBErrorCode::SYSTEM_ERROR;
	}
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

	SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

	SQLWCHAR* connectionStr = (SQLWCHAR*)s_dbConnectionInfo.GetWString();
	retCode = SQLDriverConnect(hDbc, NULL, connectionStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

	if (!SQL_SUCCEEDED(retCode)) {
		// something to do
		return (unsigned)DBErrorCode::HAVE_TO_ADD;
	}

	printf("Successfully connected to the database.\n");
	
	HANDLE objects[2];
	objects[0] = s_hQueryEvents[threadID];
	objects[1] = s_hEndEvent;

	while (true) {
		DWORD result = WaitForMultipleObjects(2, objects, FALSE, INFINITE);
		
		switch (result) {
		case WAIT_OBJECT_0: // Query Event
			break;
		case WAIT_OBJECT_0 + 1: // Close Event
			goto CLOSE_SEQUENT;
			break;
		}
		
		

		DBEvent dbEvent;
		BOOL nonempty = s_queryCQueue.TryGetAndPop(&dbEvent);
		while (nonempty) {
			SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
			if (nonempty) {
				switch (dbEvent.code) {
				case DBEventCode::QUERY_VALIDATION:
					HandleQueryPlayerInfo(hStmt, dbEvent);
					break;
				case DBEventCode::QUERY_LOAD_STAT:
					HandleQueryLoadPlayerStat(hStmt, dbEvent);
					break;
				case DBEventCode::QUERY_UPDATE_STAT:
					HandleQueryUpdatePlayerStat(hStmt, dbEvent);
					break;
				}
			}
			SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

			nonempty = s_queryCQueue.TryGetAndPop(&dbEvent);
		}
	}

CLOSE_SEQUENT:

	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

	return 0;
}

void HandleQueryPlayerInfo(HANDLE hStmt, DBEvent dbEvent)
{
	SQLRETURN retCode;

	DBQueryValidation* queryPlayerInfo = (DBQueryValidation*)dbEvent.pEvent;
	retCode = SQLExecDirect(hStmt, (SQLWCHAR*)queryPlayerInfo->GetQuery(), SQL_NTS);
	
	SQLLEN indicator;
	int userID = 0;
	
	if (SQL_SUCCEEDED(retCode)) {
		SQLFetch(hStmt);
		retCode = SQLGetData(hStmt, 1, SQL_INTEGER, &userID, sizeof(int), &indicator);
		if (retCode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO) {
			if (indicator == SQL_NULL_DATA) {
				queryPlayerInfo->SetResult(DBResultCode::FAIL_VALIDATION_TYPE_MISMATCH, -1);
			}
			else {
				queryPlayerInfo->SetResult(DBResultCode::SUCCESS, userID);
				
			}
		}
		else {
			queryPlayerInfo->SetResult(DBResultCode::FAIL_VALIDATION_TYPE_MISMATCH, -1);
		}
	}
	else {
		printf("Failed to execute query.\n");
		__debugbreak();
	}

	PushResult(dbEvent);
}

void HandleQueryLoadPlayerStat(HANDLE hStmt, DBEvent dbEvent)
{
	SQLRETURN retCode;
	
	DBQueryLoadStat* queryLoadStat = (DBQueryLoadStat*)dbEvent.pEvent;
	retCode = SQLExecDirect(hStmt, (SQLWCHAR*)queryLoadStat->GetQuery(), SQL_NTS);

	SQLLEN indicator;
	int hitCount = -1;
	int killCount = -1;
	int deathCount = -1;

	if (SQL_SUCCEEDED(retCode)) {
		SQLFetch(hStmt);
		retCode = SQLGetData(hStmt, 1, SQL_INTEGER, &hitCount, sizeof(int), &indicator);
		if (retCode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO) {
			if (indicator == SQL_NULL_DATA) {
				hitCount = -1;
				queryLoadStat->SetResult(DBResultCode::FAIL_VALIDATION_TYPE_MISMATCH, -1, -1, -1);
			}
		}
		else {
			hitCount = -1;
			queryLoadStat->SetResult(DBResultCode::FAIL_VALIDATION_TYPE_MISMATCH, -1, -1, -1);
		}

		retCode = SQLGetData(hStmt, 2, SQL_INTEGER, &killCount, sizeof(int), &indicator);
		if (retCode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO) {
			if (indicator == SQL_NULL_DATA) {
				hitCount = -1;
				queryLoadStat->SetResult(DBResultCode::FAIL_VALIDATION_TYPE_MISMATCH, -1, -1, -1);
			}
		}
		else {
			hitCount = -1;
			queryLoadStat->SetResult(DBResultCode::FAIL_VALIDATION_TYPE_MISMATCH, -1, -1, -1);
		}

		retCode = SQLGetData(hStmt, 3, SQL_INTEGER, &deathCount, sizeof(int), &indicator);
		if (retCode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO) {
			if (indicator == SQL_NULL_DATA) {
				hitCount = -1;
				queryLoadStat->SetResult(DBResultCode::FAIL_VALIDATION_TYPE_MISMATCH, -1, -1, -1);
			}
		}
		else {
			hitCount = -1;
			queryLoadStat->SetResult(DBResultCode::FAIL_VALIDATION_TYPE_MISMATCH, -1, -1, -1);
		}

		if (hitCount >= 0 && killCount >= 0 && deathCount >= 0) {
			queryLoadStat->SetResult(DBResultCode::SUCCESS, hitCount, killCount, deathCount);
		}
	}
	else {
		printf("Failed to execute query.\n");
		__debugbreak();
	}

	PushResult(dbEvent);
}

void HandleQueryUpdatePlayerStat(HANDLE hStmt, DBEvent dbEvent)
{
	SQLRETURN retCode;

	DBQueryUpdateStat* queryUpdateStat = (DBQueryUpdateStat*)dbEvent.pEvent;
	
	retCode = SQLExecDirect(hStmt, (SQLWCHAR*)queryUpdateStat->GetQuery(), SQL_NTS);

	SQLLEN rowsAffected;

	if (retCode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO) {
		retCode = SQLRowCount(hStmt, &rowsAffected);
		if (retCode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO) {
			if (rowsAffected == 1) {
				queryUpdateStat->SetResult(DBResultCode::SUCCESS);
			}
			else {
				queryUpdateStat->SetResult(DBResultCode::FAIL_UPDATE_STAT_WRONG_AFFECTED_COUNT);
			}
		}
		else {
			// 오류 상황
			__debugbreak();
		}
	}
	else if (retCode == SQL_NO_DATA) {
		queryUpdateStat->SetResult(DBResultCode::FAIL_UPDATE_STAT_NO_DATA_WITH_PLAYER_ID);
	}
	else {
		// 오류 상황
		__debugbreak();
	}

	PushResult(dbEvent);
}

void PushResult(DBEvent dbEvent)
{
	AcquireSRWLockExclusive(&s_resultQueueLock);
	s_pResultCQueueBack->Push(dbEvent);
	ReleaseSRWLockExclusive(&s_resultQueueLock);
}



