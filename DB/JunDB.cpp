#include "pch.h"

#include "JunDB.h"
#include <atlconv.h>


std::wstring dbConnectionInfo;

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
	USES_CONVERSION;
	dbConnectionInfo = L"DRIVER={SQL Server};SERVER=";
	dbConnectionInfo.append(A2W(connectionInfo.ip.c_str()));
	dbConnectionInfo.append(L",");
	dbConnectionInfo.append(A2W(connectionInfo.port.c_str()));
	dbConnectionInfo.append(L";DATABASE=");
	dbConnectionInfo.append(A2W(connectionInfo.dbName.c_str()));
	dbConnectionInfo.append(L";UID=");
	dbConnectionInfo.append(A2W(connectionInfo.uid.c_str()));
	dbConnectionInfo.append(L";PWD=");
	dbConnectionInfo.append(A2W(connectionInfo.pwd.c_str()));
	dbConnectionInfo.append(L";");

	_hThreads = new HANDLE[numThreads];
	_hQueryEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("QUERY"));
	_endEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	for (int i = 0; i < numThreads; ++i) {
		_hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadDB, &_hQueryEvent, 0, NULL);
	}
	
	return DBErrorCode::NONE;
}

DBErrorCode JunDB::End()
{
	SetEvent(_endEvent);
	DWORD result = WaitForMultipleObjects(_numThreads, _hThreads, TRUE, 10000); // wait 10s
	for (int i = 0; i < _numThreads; ++i) {
		CloseHandle(_hThreads[i]);
	}
	CloseHandle(_endEvent);
	if (result == WAIT_OBJECT_0) {
		delete[] _hThreads;
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

	_queryCQueue.Push(ev);
	SetEvent(_hQueryEvent);
}

void JunDB::LoadStat(const WCHAR* ID)
{
	DBEvent ev;
	ev.code = DBEventCode::QUERY_LOAD_STAT;

	DBQueryLoadStat* query = new DBQueryLoadStat(ID);

	ev.pEvent = (void*)query;

	_queryCQueue.Push(ev);
	SetEvent(_hQueryEvent);
}

void JunDB::StoreStat(const WCHAR* ID, int hitCount, int killCount, int deathCount)
{
	DBEvent ev;
	ev.code = DBEventCode::QUERY_STORE_STAT;

	DBQueryUpdateStat* query = new DBQueryUpdateStat(ID, hitCount, killCount, deathCount);
	ev.pEvent = (void*)query;

	_queryCQueue.Push(ev);
	SetEvent(_hQueryEvent);
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

	//SQLWCHAR connectionStr[] = L"DRIVER={SQL Server};SERVER=192.168.0.33,1433;DATABASE=tankDB;UID=pearson;PWD=mathematics;";
	SQLWCHAR* connectionStr = (SQLWCHAR*)dbConnectionInfo.c_str();
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



