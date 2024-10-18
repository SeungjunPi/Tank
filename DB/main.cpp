#include <windows.h>
#include <stdio.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>

int main(int argc, char* args[])
{
	if (argc != 1) {
		return 1;
	}

	SQLHENV hEnv = nullptr;
	SQLHDBC hDbc = nullptr;
	SQLHSTMT hStmt = nullptr;
	SQLRETURN retCode;

	retCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	printf("SQLAllocHandle retCode=%d\n", retCode);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

	SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

	SQLWCHAR connectionStr[] = L"DRIVER={SQL Server};SERVER=localhost,1433;DATABASE=tankDB;UID=sa;PWD=mathematics;";
	retCode = SQLDriverConnect(hDbc, NULL, connectionStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);


	//retCode = SQLDriverConnect(hDbc, NULL, connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

	if (SQL_SUCCEEDED(retCode)) {
		printf("Successfully connected to the database.\n");
		SQLCHAR sqlData[256];
		SQLLEN indicator;

		{
			SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

			retCode = SQLExecDirect(hStmt, (SQLWCHAR*)L"SELECT * from Players;", SQL_NTS);

			if (SQL_SUCCEEDED(retCode)) {
				while (SQLFetch(hStmt) == SQL_SUCCESS) {
					for (int i = 1; ; ++i) {
						retCode = SQLGetData(hStmt, i, SQL_C_WCHAR, sqlData, sizeof(sqlData), &indicator);

						if (retCode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO) {
							if (indicator == SQL_NULL_DATA) {
								wprintf(L"NULL\t");
							}
							else {
								wprintf(L"%s\t", (WCHAR*)sqlData);
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

			SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		}
	}
	else {
		printf("Failed to connect to the database.\n");
	}

	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

	return 0;
}

