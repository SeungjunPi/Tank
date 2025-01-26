#pragma once

#include "NetCoreAliases.h"

class Dummy;


class DummyManager
{
public:
	DummyManager();
	~DummyManager();

	void CreatePlayer();
	void RemovePlayer();

	Dummy* GetDummyBySessionID(SessionID sessionID);

	void OnFrame();

private:
	int _countPlayer = 0;
	Dummy* _dummy = nullptr;
};

