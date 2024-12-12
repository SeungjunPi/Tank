#pragma once

#include "CQueue.h"
#include "DBStruct.h"
#include "IJunDB.h"
#include "LiteWString.h"


class JunDB: public IJunDB
{
public:
	DBErrorCode Start(const DBConnectionInfo connectionInfo, SHORT numThreads) override;
	DBErrorCode End() override;

	virtual void ValidatePlayerInfo(const WCHAR* ID, const WCHAR* pw) override;
	virtual void LoadStat(const WCHAR* ID) override;
	virtual void UpdateStat(const WCHAR* ID, int hitCount, int killCount, int deathCount) override;

	virtual CQueue<DBEvent>* BeginHandleResult() override;
	virtual void EndHandleResult() override;

private:
	DWORD _numThreads = 0;
	HANDLE* s_hThreads = nullptr;

	void SetQueryEvents() const;
};

void CreateJunDB(IJunDB** out_pJunDB);
void TerminateJunDB(IJunDB* pJunDB);

