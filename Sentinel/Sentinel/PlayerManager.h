
#pragma once

#include "stdafx.h"
#include "Player.h"
#include "PointerTable.h"


class PlayerManager
{
public:
	void Initiate(int maxNumPlayers);
	void Terminate();

	Player* TryCreatePlayer(UINT32 id);
	BOOL TryDeletePlayer(UINT32 id);

	int GetCapacity() const;

	UINT16 GetAllKeys(UINT32* out);

	Player* GetPlayer(UINT32 id) const;

private:
	PointerTable _playerTable;
	UINT32* _usedKeys = nullptr;
	UINT16 _countPlayers = 0;
};

