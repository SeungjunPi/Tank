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

	virtual void ValidateUserInfo(const WCHAR* ID, const WCHAR* pw, UINT32 sessionID) override;
	virtual void LoadStat(UINT32 sessionID, int userID) override;
	virtual void UpdateStat(int userID, int hitCount, int killCount, int deathCount) override;

	// DB 쿼리의 결과 처리 시작  
	virtual void BeginHandleResult() override;

	// 이 함수를 통해 모든 쌓인 결과를 처리해야 함. 즉, FALSE가 나올 때 까지 실행해야 함. 
	virtual BOOL TryGetEvent(DBEvent* out) override;

	// 이 함수를 실행하기 전, TryGetEvent 함수를 통해 반드시 쌓인 결과를 모두 처리해야 함.
	virtual void EndHandleResult() override;

private:
	DWORD _numThreads = 0;
	HANDLE* s_hThreads = nullptr;

	void SetQueryEvents() const;
};

void CreateJunDB(IJunDB** out_pJunDB);
void TerminateJunDB(IJunDB* pJunDB);

