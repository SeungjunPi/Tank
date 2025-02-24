#pragma once

#include <unordered_map>

#include "DummyPch.h"
#include "NetCoreAliases.h"
#include "DummyAliases.h"

class Dummy;

const ULONGLONG DUMMY_ADD_DELAY = 100;

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

	void Tick();

private:
	const int MAX_DUMMY_COUNT = 400;
	int _countDummy = 0;
	Dummy* _pDummyMaster = nullptr;

	std::unordered_map<SessionID, Dummy*> _dummyMap;

	ULONGLONG _lastDummyAddTick = 0;

	std::wstring* ids;
	std::wstring* pws;
};

