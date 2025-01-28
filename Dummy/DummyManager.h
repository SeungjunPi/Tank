#pragma once

#include <unordered_map>

#include "TankPch.h"
#include "NetCoreAliases.h"
#include "TankAliases.h"

class Dummy;

const ULONGLONG DUMMY_ADD_DELAY = 5000;

class DummyManager
{
public:
	DummyManager();
	~DummyManager();

	void Initiate();
	void Shutdown();

	void CreateDummyOrNot();
	void RemoveDummy(SessionID sessionID);

	bool IsMaster(SessionID sessionID) const;

	// 없는 더미의 ID인 경우 nullptr 반환
	Dummy* GetDummyBySessionID(SessionID sessionID);
	Dummy* GetDummyByUserID(UserDBIndex userID);

	void OnFrame();

private:
	int _countDummy = 0;
	Dummy* _pDummyMaster = nullptr;

	std::unordered_map<SessionID, Dummy*> _dummyMap;

	ULONGLONG _lastDummyAddTick = 0;
};

