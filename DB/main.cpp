

#include "JunDB.h"

//
//#include <windows.h>
//#include <stdio.h>
//#include <sqlext.h>
//#include <sqltypes.h>
//#include <sql.h>

//#include "DBStructs.h"
//#include <string>
//#include <atlconv.h>
//

//
//struct DBConnectionInfo
//{
//	std::string ip;
//	std::string port;
//	std::string dbName;
//	std::string uid;
//	std::string pwd;
//};

#include "LiteWString.h"

int main(int argc, char* args[])
{
	if (argc != 1) {
		return 1;
	}

	DBConnectionInfo dbConnInfo;
	{
		char buffer[100];

		printf_s("IP: ");
		char* res = fgets(buffer, 16, stdin);
		if (res == NULL) {
			feof(stdin);
			ferror(stdin);
		}
		buffer[strnlen_s(buffer, 100) - 1] = '\0';
		dbConnInfo.ip = std::string(buffer);

		printf_s("port: ");
		res = fgets(buffer, 16, stdin);
		if (res == NULL) {
			feof(stdin);
			ferror(stdin);
		}
		buffer[strnlen_s(buffer, 100) - 1] = '\0';
		dbConnInfo.port = std::string(buffer);

		printf_s("DB Name: ");
		res = fgets(buffer, 16, stdin);
		if (res == NULL) {
			feof(stdin);
			ferror(stdin);
		}
		buffer[strnlen_s(buffer, 100) - 1] = '\0';
		dbConnInfo.dbName = std::string(buffer);


		printf_s("ID: ");
		res = fgets(buffer, 16, stdin);
		if (res == NULL) {
			feof(stdin);
			ferror(stdin);
		}
		buffer[strnlen_s(buffer, 100) - 1] = '\0';
		dbConnInfo.uid = std::string(buffer);
		
		printf_s("PW: ");
		res = fgets(buffer, 16, stdin);
		if (res == NULL) {
			feof(stdin);
			ferror(stdin);
		}
		buffer[strnlen_s(buffer, 100) - 1] = '\0';
		dbConnInfo.pwd = std::string(buffer);
	}

	IJunDB* pDb;
	CreateJunDB(&pDb);

	pDb->Start(dbConnInfo, 1);	

	pDb->ValidatePlayerInfo(L"player001", L"passw0rd001");
	pDb->LoadStat(L"player001");
	pDb->UpdateStat(L"player001", 12, 3, 2);
	
	Sleep(100000);

	auto queue = pDb->BeginHandleResult();
	DBEvent pEvent;
	queue->TryGetAndPop(&pEvent);
	queue->TryGetAndPop(&pEvent);
	queue->TryGetAndPop(&pEvent);

	pDb->EndHandleResult();

	TerminateJunDB(pDb);


	//SQLHENV hEnv = nullptr;
	//SQLHDBC hDbc = nullptr;
	//SQLHSTMT hStmt = nullptr;
	//SQLRETURN retCode;

	//retCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	//printf("SQLAllocHandle retCode=%d\n", retCode);
	//SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

	//SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

	//DBConnectionInfo dbConnInfo;
	//std::wstring dbConnectionInfo;
	//{
	//	char buffer[100];

	//	printf_s("IP: ");
	//	char* res = fgets(buffer, 16, stdin);
	//	if (res == NULL) {
	//		feof(stdin);
	//		ferror(stdin);
	//	}
	//	buffer[strnlen_s(buffer, 100) - 1] = '\0';
	//	dbConnInfo.ip = std::string(buffer);

	//	printf_s("port: ");
	//	res = fgets(buffer, 16, stdin);
	//	if (res == NULL) {
	//		feof(stdin);
	//		ferror(stdin);
	//	}
	//	buffer[strnlen_s(buffer, 100) - 1] = '\0';
	//	dbConnInfo.port = std::string(buffer);

	//	printf_s("DB Name: ");
	//	res = fgets(buffer, 16, stdin);
	//	if (res == NULL) {
	//		feof(stdin);
	//		ferror(stdin);
	//	}
	//	buffer[strnlen_s(buffer, 100) - 1] = '\0';
	//	dbConnInfo.dbName = std::string(buffer);


	//	printf_s("ID: ");
	//	res = fgets(buffer, 16, stdin);
	//	if (res == NULL) {
	//		feof(stdin);
	//		ferror(stdin);
	//	}
	//	buffer[strnlen_s(buffer, 100) - 1] = '\0';
	//	dbConnInfo.uid = std::string(buffer);
	//	
	//	printf_s("PW: ");
	//	res = fgets(buffer, 16, stdin);
	//	if (res == NULL) {
	//		feof(stdin);
	//		ferror(stdin);
	//	}
	//	buffer[strnlen_s(buffer, 100) - 1] = '\0';
	//	dbConnInfo.pwd = std::string(buffer);


	//	USES_CONVERSION;
	//	dbConnectionInfo = L"DRIVER={SQL Server};SERVER=";
	//	dbConnectionInfo.append(A2W(dbConnInfo.ip.c_str()));
	//	dbConnectionInfo.append(L",");
	//	dbConnectionInfo.append(A2W(dbConnInfo.port.c_str()));
	//	dbConnectionInfo.append(L";DATABASE=");
	//	dbConnectionInfo.append(A2W(dbConnInfo.dbName.c_str()));
	//	dbConnectionInfo.append(L";UID=");
	//	dbConnectionInfo.append(A2W(dbConnInfo.uid.c_str()));
	//	dbConnectionInfo.append(L";PWD=");
	//	dbConnectionInfo.append(A2W(dbConnInfo.pwd.c_str()));
	//	dbConnectionInfo.append(L";");
	//}


	////SQLWCHAR connectionStr[] = L"DRIVER={SQL Server};SERVER=192.168.0.33,1433;DATABASE=tankDB;UID=pearson;PWD=mathematics;";
	//SQLWCHAR* connectionStr = (SQLWCHAR*)dbConnectionInfo.c_str();
	//retCode = SQLDriverConnect(hDbc, NULL, connectionStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);


	////retCode = SQLDriverConnect(hDbc, NULL, connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

	//if (SQL_SUCCEEDED(retCode)) {
	//	printf("Successfully connected to the database.\n");
	//	SQLCHAR sqlData[256];
	//	SQLLEN indicator;
	//	
	//	SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

	//	const WCHAR* query1 = nullptr;

	//	//DBQueryPlayerInfo queryPlayerInfo(L"player001", L"passw0rd001");
	//	//query1 = queryPlayerInfo.GetQuery();

	//	//DBQueryLoadStat queryLoadStat(L"player002");
	//	//query1 = queryLoadStat.GetQuery();
	//	//
	//	DBQueryUpdateStat queryUpdateStat(L"player003", 16, 2, 5);
	//	query1 = queryUpdateStat.GetQuery();

	//	retCode = SQLExecDirect(hStmt, (SQLWCHAR*)query1, SQL_NTS);
	//	//retCode = SQLExecDirect(hStmt, (SQLWCHAR*)L"select case when exists (select 1 from tankDB.dbo.Players where PlayerID = 'player001       ' and Password = 'passw0rd001     ') then 1 else 0 end as existance;", SQL_NTS);

	//	if (SQL_SUCCEEDED(retCode)) {
	//		while (SQLFetch(hStmt) == SQL_SUCCESS) {
	//			for (int i = 1; ; ++i) {
	//				retCode = SQLGetData(hStmt, i, SQL_C_WCHAR, sqlData, sizeof(sqlData), &indicator);

	//				if (retCode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO) {
	//					if (indicator == SQL_NULL_DATA) {
	//						wprintf(L"NULL ");
	//					}
	//					else {
	//						wprintf(L"%s ", (WCHAR*)sqlData);
	//					}
	//				}
	//				else {
	//					// Handle Errors
	//					break;
	//				}


	//			}
	//			wprintf(L"\n");
	//		}
	//	}
	//	else {
	//		printf("Failed to execute query.\n");
	//	}
	//		
	//		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	//	
	//	
	//}
	//else {
	//	printf("Failed to connect to the database.\n");
	//}

	//SQLDisconnect(hDbc);
	//SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	//SQLFreeHandle(SQL_HANDLE_ENV, hEnv);



	return 0;
}

