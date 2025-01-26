
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
	std::wstring name(L"player008");
	std::wstring pw(L"passw0rd008");
	_dummy = DNew Dummy(name, pw);
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
