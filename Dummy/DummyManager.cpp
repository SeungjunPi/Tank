
#include "DummyManager.h"
#include "TankPch.h"
#include "Dummy.h"

DummyManager::DummyManager()
{
}

DummyManager::~DummyManager()
{
}

void DummyManager::CreatePlayer()
{
	_dummy = DNew Dummy(L"player008", L"passw0rd008");
}

void DummyManager::RemovePlayer()
{
	if (_dummy != nullptr) {
		delete _dummy;
	}
}

Dummy* DummyManager::GetDummyBySessionID(SessionID sessionID)
{
	return _dummy;
}

void DummyManager::OnFrame()
{
	// Player 추가
	if (_dummy == nullptr) {
		CreatePlayer();
	}

	// Player 동작
	_dummy->OnFrame();
	
	// Player 제거
}
