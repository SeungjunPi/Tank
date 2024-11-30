#pragma once

#include "CQueue.h"
#include "DBStructs.h"
#include "IJunDB.h"


class JunDB : public IJunDB
{
public:
	DBErrorCode Start(const DBConnectionInfo connectionInfo, SHORT numThreads) override;
	DBErrorCode End() override;

	void ValidatePlayerInfo(const WCHAR* ID, const WCHAR* pw) override;
	void LoadStat(const WCHAR* ID) override;
	void StoreStat(const WCHAR* ID, int hitCount, int killCount, int deathCount) override;
private:
	DWORD _numThreads;
	HANDLE* _hThreads;
	
	std::wstring _dbConnectionInfo;

	HANDLE _hQueryEvent;
	HANDLE _endEvent;

	CQueue<DBEvent> _queryCQueue;
	CQueue<DBEvent> _resultCQueue;
};


void CreateJunDB(IJunDB** out_pJunDB);
void TerminateJunDB(IJunDB* pJunDB);

