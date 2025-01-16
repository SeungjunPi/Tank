#include "PlayerManager.h"

void PlayerManager::Initiate(int maxNumPlayers)
{
	_playerTable.Initiate(maxNumPlayers);
	_usedDbIndexes = DNew UINT32[maxNumPlayers];
	_pUserIdentifierManager = DNew UserIdentifierManager;
}

void PlayerManager::Terminate()
{
	for (UINT32 i = 0; i < _countPlayers; ++i) {
		UINT32 key = _usedDbIndexes[i];
		void* ptr = _playerTable.Pop(key);
		assert(ptr != nullptr);
		delete ptr;
	}
	delete[] _usedDbIndexes;
	delete _pUserIdentifierManager;

	bool terminated = _playerTable.Terminate();
	assert(terminated);
}

Player* PlayerManager::TryCreatePlayer(SessionID sessionID, UserDBIndex userDBIndex, int hitCount, int killCount, int deathCount)
{
	Player* newPlayer = DNew Player(sessionID, userDBIndex);
	newPlayer->_hitCount = hitCount;
	newPlayer->_killCount = killCount;
	newPlayer->_deathCount = deathCount;
	
	_pUserIdentifierManager->AddUserIdentifier(userDBIndex, sessionID);


	bool isInserted = _playerTable.Insert(userDBIndex, newPlayer);
	if (!isInserted) {
		delete newPlayer;
		return nullptr;
	}
	
	_usedDbIndexes[_countPlayers] = userDBIndex;
	_countPlayers++;

	return newPlayer;
}

BOOL PlayerManager::TryDeletePlayerBySessionID(SessionID sessionID)
{
	UserDBIndex dbIndex = _pUserIdentifierManager->GetUserDBIndex(sessionID);
	BOOL success = TryDeletePlayerByUserIndex(dbIndex);
	return success;
}

BOOL PlayerManager::TryDeletePlayerByUserIndex(UserDBIndex userIndex)
{
	SessionID sessionID = _pUserIdentifierManager->GetUserSessionID(userIndex);
	BOOL success = _pUserIdentifierManager->RemoveUserIdentifierBySessionID(sessionID);
	assert(success);
	Player* pPlayer = (Player*)_playerTable.Pop(userIndex);
	if (pPlayer == nullptr) {
		return false;
	}

	

	delete pPlayer;

	for (UINT32 i = 0; i < _countPlayers; ++i) {
		if (_usedDbIndexes[i] == userIndex) {
			_countPlayers--;
			_usedDbIndexes[i] = _usedDbIndexes[_countPlayers];
			_usedDbIndexes[_countPlayers] = 0;
			break;
		}

		assert(i < _countPlayers - 1);
	}

	return true;
}

int PlayerManager::GetCapacity() const
{
	return _playerTable.GetCapacity();
}

UINT16 PlayerManager::GetAllUserIndexes(UserDBIndex* out)
{
	memcpy(out, _usedDbIndexes, sizeof(UserDBIndex) * _countPlayers);
	return _countPlayers;
}

UINT16 PlayerManager::GetAllSessionIDs(SessionID* out)
{
	SessionID* dst = out;
	for (int i = 0; i < _countPlayers; ++i) {
		UserDBIndex key = _usedDbIndexes[i];
		SessionID sessionID = _pUserIdentifierManager->GetUserSessionID(key);
		*dst = sessionID;
		++dst;
	}
	return _countPlayers;
}

Player* PlayerManager::GetPlayerByUserIndex(UserDBIndex key) const
{
	return (Player*)_playerTable.Get(key);
}

Player* PlayerManager::GetPlayerBySessionID(SessionID sessionID) const
{
	UserDBIndex dbIndex = _pUserIdentifierManager->GetUserDBIndex(sessionID);
	if (dbIndex == 0) {
		return nullptr;
	}
	Player* pPlayer = GetPlayerByUserIndex(dbIndex);
	return pPlayer;
}

void PlayerManager::IncreaseHitCount(UserDBIndex userIndex)
{
	Player* pPlayer = GetPlayerByUserIndex(userIndex);
	pPlayer->IncreaseHitCount();
}

void PlayerManager::IncreaseKillCount(UserDBIndex userIndex)
{
	Player* pPlayer = GetPlayerByUserIndex(userIndex);
	pPlayer->IncreaseKillCount();
}

void PlayerManager::IncreaseDeathCount(UserDBIndex userIndex)
{
	Player* pPlayer = GetPlayerByUserIndex(userIndex);
	pPlayer->IncreaseDeathCount();
}

