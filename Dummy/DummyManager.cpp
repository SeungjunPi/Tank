
#include "DummyManager.h"
#include "TankPch.h"
#include "Dummy.h"

DummyManager::DummyManager()
{
}

DummyManager::~DummyManager()
{
}

void DummyManager::Initiate()
{
	if (_pDummyMaster != nullptr) {
		__debugbreak();
	}
	std::wstring name(L"player008");
	std::wstring pw(L"passw0rd008");
	_pDummyMaster = DNew Dummy(name, pw);
}

void DummyManager::Shutdown()
{
	if (_pDummyMaster == nullptr) {
		__debugbreak();
	}

	delete _pDummyMaster;
}

void DummyManager::CreateDummy()
{
	__debugbreak();
}

void DummyManager::RemoveDummy(SessionID sessionID)
{
	__debugbreak();
	
}

bool DummyManager::IsMaster(SessionID sessionID) const
{
	if (_pDummyMaster == nullptr) {
		__debugbreak();
	}

	return _pDummyMaster->GetSessionID() == sessionID;
}

Dummy* DummyManager::GetDummyBySessionID(SessionID sessionID)
{
	return _pDummyMaster;
}

void DummyManager::OnFrame()
{
	// Player 추가
	if (_pDummyMaster == nullptr) {
		__debugbreak();
	}

	// Player 동작
	_pDummyMaster->OnFrame();
	
	// Player 제거
}
