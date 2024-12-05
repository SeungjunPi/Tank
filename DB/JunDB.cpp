#include "pch.h"

#include "JunDB.h"
#include <atlconv.h>


static LiteWString s_dbConnectionInfo;

static HANDLE* s_hQueryEvents = nullptr;
static HANDLE s_hEndEvent;

static CQueue<DBEvent> s_queryCQueue;
static CQueue<DBEvent> s_resultCQueueFront;
static CQueue<DBEvent> s_resultCQueueBack;

static SRWLOCK s_resultQueueLock;

unsigned WINAPI ThreadDB(LPVOID pParam);
void HandleQueryPlayerInfo(void* pQueryPlayerInfo);
void HandleQueryLoadPlayerStat(void* pQueryLoadPlayerStat);
void HandleQueryStorePlayerStat(void* pQueryStorePlayerStat);



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

	char buffer[LiteWString::MAX_WSTR_LENGTH + 1];
		
	size_t length = sprintf_s(buffer, LiteWString::MAX_WSTR_LENGTH, 
		"DRIVER={SQL Server};SERVER=%s, %s; DATABASE=%s; UID=%s; PWD=%s;",
		connectionInfo.ip.c_str(),
		connectionInfo.port.c_str(),
		connectionInfo.dbName.c_str(),
		connectionInfo.uid.c_str(),
		connectionInfo.pwd.c_str());

	USES_CONVERSION_EX;
	s_dbConnectionInfo.Append(A2W_EX(buffer, length));

	s_hThreads = new HANDLE[numThreads];
	s_hQueryEvents = new HANDLE[numThreads];
		
	s_hEndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	s_resultQueueLock = SRWLOCK_INIT;

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
	for (int i = 0; i < _numThreads; ++i) {
		CloseHandle(s_hThreads[i]);
	}
	CloseHandle(s_hEndEvent);
	if (result == WAIT_OBJECT_0) {
		delete[] s_hThreads;
		return DBErrorCode::NONE;
	}
	return DBErrorCode::NONE;
}

void JunDB::ValidatePlayerInfo(const WCHAR* ID, const WCHAR* pw)
{
	DBEvent ev;
	ev.code = DBEventCode::QUERY_PLAYER_INFO;

	DBQueryPlayerInfo* query = new DBQueryPlayerInfo(ID, pw);
	ev.pEvent = (void*)query;

	s_queryCQueue.Push(ev);
	SetEvent(s_hQueryEvents);
}

void JunDB::LoadStat(const WCHAR* ID)
{
	DBEvent ev;
	ev.code = DBEventCode::QUERY_LOAD_STAT;

	DBQueryLoadStat* query = new DBQueryLoadStat(ID);

	ev.pEvent = (void*)query;

	s_queryCQueue.Push(ev);
	SetEvent(s_hQueryEvents);
}

void JunDB::StoreStat(const WCHAR* ID, int hitCount, int killCount, int deathCount)
{
	DBEvent ev;
	ev.code = DBEventCode::QUERY_STORE_STAT;

	DBQueryUpdateStat* query = new DBQueryUpdateStat(ID, hitCount, killCount, deathCount);
	ev.pEvent = (void*)query;

	s_queryCQueue.Push(ev);
	SetEvent(s_hQueryEvents);
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

	SQLCHAR sqlData[256];
	SQLLEN indicator;
	
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
		
		SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

		DBEvent dbEvent;
		BOOL nonempty = s_queryCQueue.TryGetAndPop(&dbEvent);
		if (nonempty) {
			switch (dbEvent.code) {
			case DBEventCode::QUERY_PLAYER_INFO:
			{
				DBQueryPlayerInfo* queryPlayerInfo = (DBQueryPlayerInfo*)dbEvent.pEvent;
				retCode = SQLExecDirect(hStmt, (SQLWCHAR*)queryPlayerInfo->GetQuery(), SQL_NTS);

				DBResultPlayerInfo resultPlayerInfo;
				

				// handle validation result

				break;
			}
			case DBEventCode::QUERY_LOAD_STAT:
			{
				DBQueryLoadStat* queryLoadStat = (DBQueryLoadStat*)dbEvent.pEvent;

				// handle load stat result

				break;
			}
			case DBEventCode::QUERY_STORE_STAT:
			{
				DBQueryUpdateStat* queryUpdateStat = (DBQueryUpdateStat*)dbEvent.pEvent;

				// handle store stat result

				break;
			}
			}

		}


		//////////////////////////////////////////////////////////////////////////////
		//retCode = SQLExecDirect(hStmt, (SQLWCHAR*)L"SELECT * from Players;", SQL_NTS);

		if (SQL_SUCCEEDED(retCode)) {
			while (SQLFetch(hStmt) == SQL_SUCCESS) {
				for (int i = 1; ; ++i) {
					retCode = SQLGetData(hStmt, i, SQL_C_WCHAR, sqlData, sizeof(sqlData), &indicator);
					if (retCode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO) {
						if (indicator == SQL_NULL_DATA) {
							wprintf(L"NULL ");
						}
						else {
							wprintf(L"%s ", (WCHAR*)sqlData);
						}
					}
					else {
						// Handle Errors
						break;
					}
				}
				wprintf(L"\n");
			}
		}
		else {
			printf("Failed to execute query.\n");
		}
		//////////////////////////////////////////////////////////////////////////////

		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	}

CLOSE_SEQUENT:

	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

	return 0;
}

void HandleQueryPlayerInfo(void* pQueryPlayerInfo)
{
	SQLHSTMT hStmt = nullptr;

	//DBQueryPlayerInfo* queryPlayerInfo = (DBQueryPlayerInfo*)dbEvent.pEvent;
	//retCode = SQLExecDirect(hStmt, (SQLWCHAR*)queryPlayerInfo->GetQuery(), SQL_NTS);
}

void HandleQueryLoadPlayerStat(void* pQueryLoadPlayerStat)
{
}

void HandleQueryStorePlayerStat(void* pQueryStorePlayerStat)
{
}



