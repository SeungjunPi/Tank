#pragma once

#include "TankPch.h"
#include "NetCoreAliases.h"
#include "TankAliases.h"

class Dummy;


class DummyManager
{
public:
	DummyManager();
	~DummyManager();

	void Initiate();
	void Shutdown();

	void CreateDummy();
	void RemoveDummy(SessionID sessionID);

	bool IsMaster(SessionID sessionID) const;

	// 없는 더미의 ID인 경우 nullptr 반환
	Dummy* GetDummyBySessionID(SessionID sessionID);
	Dummy* GetDummyByUserID(UserDBIndex userID);

	void OnFrame();

private:
	int _countPlayer = 0;
	Dummy* _pDummyMaster = nullptr;
};

