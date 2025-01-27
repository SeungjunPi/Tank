
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
	_pDummyMaster->ConnectToServer();
	_lastDummyAddTick = GetTickCount64();
}

void DummyManager::Shutdown()
{
	if (_pDummyMaster == nullptr) {
		__debugbreak();
	}

	for (auto& it : _dummyMap) {
		delete it.second;
	}
	_dummyMap.clear();

	delete _pDummyMaster;
}

void DummyManager::CreateDummyOrNot()
{
	if (_lastDummyAddTick + DUMMY_ADD_DELAY > g_currentGameTick) {
		return;
	}
	_lastDummyAddTick = g_currentGameTick;
	if (_countDummy > 0) {
		return;
	}
	Dummy* pDummy = DNew Dummy(L"player009", L"passw0rd009");
	SessionID sessionID = pDummy->ConnectToServer();
	_dummyMap.emplace(sessionID, pDummy);
	++_countDummy;
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

	auto it = _dummyMap.find(sessionID);
	if (it == _dummyMap.end()) {
		return nullptr;
	}
	
	return it->second;
}

Dummy* DummyManager::GetDummyByUserID(UserDBIndex userID)
{
	if (_pDummyMaster->GetUserID() == userID) {
		return _pDummyMaster;
	}
	
	// 전부 순회하는건 비효율적이므로 미리 map을 하나 더 만들어놓아야 할 듯
	for (auto& it : _dummyMap) {
		if (it.second->GetUserID() == userID) {
			return it.second;
		}
	}

	return nullptr;
}

void DummyManager::OnFrame()
{
	// Player 추가
	CreateDummyOrNot();

	// Player 동작
	_pDummyMaster->OnFrame();
	for (auto& it : _dummyMap) {
		it.second->OnFrame();
	}
	
	// Player 제거
}
