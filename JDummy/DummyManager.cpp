
#include "DummyManager.h"
#include "TankPch.h"
#include "Dummy.h"
#include "Global.h"

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

	ids = DNew std::wstring[MAX_DUMMY_COUNT];
	pws = DNew std::wstring[MAX_DUMMY_COUNT];
	int dummyIndex = 100;
	for (int i = 0; i < MAX_DUMMY_COUNT; ++i) {
		ids[i] += L"player";
		ids[i] += std::to_wstring(dummyIndex);

		pws[i] += L"passw0rd";
		pws[i] += std::to_wstring(dummyIndex);

		++dummyIndex;
	}
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

	delete[] pws;
	delete[] ids;

	delete _pDummyMaster;
}

void DummyManager::CreateDummyOrNot()
{
	if (_lastDummyAddTick + DUMMY_ADD_DELAY > g_currentGameTick) {
		return;
	}
	_lastDummyAddTick = g_currentGameTick;
	if (_countDummy >= MAX_DUMMY_COUNT) {
		return;
	}

	printf("Dummy %d created.\n", _countDummy);

	Dummy* pDummy = DNew Dummy(ids[_countDummy], pws[_countDummy]);
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

void DummyManager::Tick()
{
	// Player 추가
	CreateDummyOrNot();

	// Player 동작
	_pDummyMaster->Tick();
	for (auto& it : _dummyMap) {
		it.second->Tick();
	}

	// Player 제거
}
