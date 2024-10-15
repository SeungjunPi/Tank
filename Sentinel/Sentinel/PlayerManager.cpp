#include "PlayerManager.h"


Player* s_player = nullptr;

void PlayerManager::Initiate(int maxNumPlayers)
{
	_playerTable.Initiate(maxNumPlayers);
	_usedKeys = new UINT32[maxNumPlayers];
}

void PlayerManager::Terminate()
{
	for (UINT16 i = 0; i < _countPlayers; ++i) {
		UINT32 key = _usedKeys[i];
		void* ptr = _playerTable.Pop(key);
		assert(ptr != nullptr);
		delete ptr;
	}
	delete[] _usedKeys;

	bool terminated = _playerTable.Terminate();
	assert(terminated);
}

Player* PlayerManager::TryCreatePlayer(UINT32 id)
{
	Player* newPlayer = new Player(id);
	s_player = newPlayer;
	bool isInserted = _playerTable.Insert(id, newPlayer);
	if (!isInserted) {
		delete newPlayer;
		return nullptr;
	}
	
	_usedKeys[_countPlayers] = id;
	_countPlayers++;

	return newPlayer;
}

BOOL PlayerManager::TryDeletePlayer(UINT32 id)
{
	Player* pPlayer = (Player*)_playerTable.Pop(id);
	if (pPlayer == nullptr) {
		return false;
	}
	delete pPlayer;

	for (UINT16 i = 0; i < _countPlayers; ++i) {
		if (_usedKeys[i] == id) {
			_countPlayers--;
			_usedKeys[i] = _usedKeys[_countPlayers];
			_usedKeys[_countPlayers] = 0;
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

UINT16 PlayerManager::GetAllKeys(UINT32* out)
{
	memcpy(out, _usedKeys, sizeof(UINT32) * _countPlayers);
	return _countPlayers;
}

Player* PlayerManager::GetPlayer(UINT32 id) const
{
	return (Player*)_playerTable.Get(id);
}
