#include "pch.h"

#include "JunDB.h"


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


DBErrorCode JunDB::Start(SHORT numThreads)
{
	_hThreads = new HANDLE[numThreads];
	_hQueryEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("QUERY"));
	_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);


	for (int i = 0; i < numThreads; ++i) {
		_hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadDB, &_hQueryEvent, 0, NULL);
		
		// TODO: Error Handling
	}
	
	return DBErrorCode::NONE;
}

DBErrorCode JunDB::End()
{
	// Send End Event to Threads

	DWORD result = WaitForMultipleObjects(_numThreads, _hThreads, TRUE, 10000); // wait 10s
	if (result == WAIT_OBJECT_0) {
		delete[] _hThreads;
		return DBErrorCode::NONE;
	}

	// TODO: Error Handling

	return DBErrorCode::NONE;
}

void JunDB::ValidatePlayerInfo(WCHAR* ID, WCHAR* pw)
{
	DBEvent ev;
	ev.code = DBEventCode::QUERY_PLAYER_INFO;

	DBQueryPlayerInfo* query = new DBQueryPlayerInfo(ID, pw);
	ev.pEvent = (void*)query;

	_queryCQueue.Push(ev);
}

void JunDB::LoadStat(WCHAR* ID)
{
	DBEvent ev;
	ev.code = DBEventCode::QUERY_LOAD_STAT;

	DBQueryLoadStat* query = new DBQueryLoadStat(ID);

	ev.pEvent = (void*)query;

	_queryCQueue.Push(ev);
}

void JunDB::StoreStat(WCHAR* ID, int hitCount, int killCount, int deathCount)
{
	DBEvent ev;
	ev.code = DBEventCode::QUERY_STORE_STAT;

	DBQueryUpdateStat* query = new DBQueryUpdateStat(ID, hitCount, killCount, deathCount);
	ev.pEvent = (void*)query;

	_queryCQueue.Push(ev);
}

unsigned WINAPI ThreadDB(LPVOID pParam)
{
	HANDLE* events = (HANDLE*)pParam;
	CQueue<DBEvent>* pQueryQueue = (CQueue<DBEvent>*)(events + 2);
	CQueue<DBEvent>* pResultQueue = (CQueue<DBEvent>*)(events + 3);


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

	SQLWCHAR connectionStr[] = L"DRIVER={SQL Server};SERVER=192.168.0.33,1433;DATABASE=tankDB;UID=pearson;PWD=mathematics;";
	retCode = SQLDriverConnect(hDbc, NULL, connectionStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

	if (!SQL_SUCCEEDED(retCode)) {
		// something to do
		return (unsigned)DBErrorCode::HAVE_TO_ADD;
	}

	printf("Successfully connected to the database.\n");

	SQLCHAR sqlData[256];
	SQLLEN indicator;
	

	while (true) {
		DWORD result = WaitForMultipleObjects(2, events, FALSE, INFINITE);
		
		switch (result) {
		case WAIT_OBJECT_0: // Query Event
			break;
		case WAIT_OBJECT_0 + 1: // Close Event
			goto CLOSE_SEQUENT;
			break;
		}
		
		SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

		DBEvent dbEvent;
		BOOL nonempty = pQueryQueue->TryGetAndPop(&dbEvent);
		if (nonempty) {
			switch (dbEvent.code) {
			case DBEventCode::QUERY_PLAYER_INFO:
			{
				DBQueryPlayerInfo* queryPlayerInfo = (DBQueryPlayerInfo*)dbEvent.pEvent;
				retCode = SQLExecDirect(hStmt, (SQLWCHAR*)queryPlayerInfo->GetQuery(), SQL_NTS);


				break;
			}
			case DBEventCode::QUERY_LOAD_STAT:
			{
				DBQueryLoadStat* queryLoadStat = (DBQueryLoadStat*)dbEvent.pEvent;

				break;
			}
			case DBEventCode::QUERY_STORE_STAT:
			{
				DBQueryUpdateStat* queryUpdateStat = (DBQueryUpdateStat*)dbEvent.pEvent;

				break;
			}
			}

		}


		//////////////////////////////////////////////////////////////////////////////
		retCode = SQLExecDirect(hStmt, (SQLWCHAR*)L"SELECT * from Players;", SQL_NTS);

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



