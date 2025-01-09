
#pragma once

#include "SentinelPch.h"
#include "Player.h"
#include "PointerTable.h"
#include "UserIdentifierManager.h"

class PlayerManager
{
public:
	void Initiate(int maxNumPlayers);
	void Terminate();

	Player* TryCreatePlayer(SessionID sessionID, UserDBIndex userIndex, int hitCount, int killCount, int deathCount);
	BOOL TryDeletePlayerBySessionID(SessionID sessionID);
	BOOL TryDeletePlayerByUserIndex(UserDBIndex userIndex);

	int GetCapacity() const;

	UINT16 GetAllUserIndexes(UserDBIndex* out);
	UINT16 GetAllSessionIDs(SessionID* out);
	

	Player* GetPlayerByUserIndex(UserDBIndex userIndex) const;
	Player* GetPlayerBySessionID(SessionID sessionID) const;
	

	void IncreaseHitCount(UserDBIndex userIndex);
	void IncreaseKillCount(UserDBIndex userIndex);
	void IncreaseDeathCount(UserDBIndex userIndex);


private:
	PointerTable32 _playerTable;

	UserIdentifierManager* _pUserIdentifierManager;

	UINT32* _usedDbIndexes = nullptr;
	UINT16 _countPlayers = 0;
};

