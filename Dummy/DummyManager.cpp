
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
	if (_pDummyMaster->GetSessionID() == sessionID) {
		return _pDummyMaster;
	}
	return nullptr;
}

Dummy* DummyManager::GetDummyByUserID(UserDBIndex userID)
{
	if (_pDummyMaster->GetUserID() == userID) {
		return _pDummyMaster;
	}
	return nullptr;
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
