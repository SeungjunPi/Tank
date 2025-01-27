#pragma once

#include "NetCoreAliases.h"

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

	void OnFrame();

private:
	int _countPlayer = 0;
	Dummy* _pDummyMaster = nullptr;
};

