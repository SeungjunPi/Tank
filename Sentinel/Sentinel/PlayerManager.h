
#pragma once

#include "stdafx.h"
#include "Player.h"
#include "PointerTable.h"


class PlayerManager
{
public:
	void Initiate(int maxNumPlayers);
	void Terminate();

	Player* TryCreatePlayer(UINT32 sessionID, UINT32 userID);
	BOOL TryDeletePlayerBySessionID(UINT32 sessionID);
	BOOL TryDeletePlayerByUserID(UINT32 userID);

	int GetCapacity() const;

	UINT16 GetAllUserIDs(UINT32* out);

	Player* GetPlayer(UINT32 userID) const;

	void IncreaseNumOtherTanksHit(UINT32 userID);
	void IncreaseNumHitsTaken(UINT32 userID);

	


private:
	PointerTable _playerTable;
	UINT32* _usedKeys = nullptr;
	UINT16 _countPlayers = 0;
};

