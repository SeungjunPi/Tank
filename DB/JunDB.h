#pragma once

#include "CQueue.h"
#include "DBStructs.h"
#include "IJunDB.h"


class JunDB : public IJunDB
{
public:
	DBErrorCode Start(SHORT numThreads) override;
	DBErrorCode End() override;

	void QueryAsync(SQLWCHAR* pQuery) override;

	void ValidatePlayerInfo(WCHAR* ID, WCHAR* pw) override;
	void LoadStat(WCHAR* ID) override;
	void StoreStat(WCHAR* ID, int hitCount, int killCount, int deathCount) override;
private:
	DWORD _numThreads;
	HANDLE* _hThreads;
	HANDLE _hQueryEvent;
	HANDLE _endEvent;

	CQueue<DBEvent> _queryCQueue;
	CQueue<DBEvent> _resultCQueue;
};


void CreateJunDB(IJunDB** out_pJunDB);
void TerminateJunDB(IJunDB* pJunDB);

