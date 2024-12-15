#pragma once

#include "stdafx.h"
#include "SentinelAliases.h"

class PlayerManager;

class Player
{
	friend PlayerManager;
public:
	Player(SessionID sessionID, UserDBIndex userID);
	SessionID GetSessionID() const;

	UserDBIndex GetUserIndex() const { return _userIndex; }

	UINT16 GetTankId() const;
	void SetTankId(UINT16 tankId);

	void IncreaseHitCount();
	void IncreaseKillCount();
	void IncreaseDeathCount();

private:
	SessionID _sessionId = 0;
	UINT32 _userIndex = 0;
	UINT16 _tankId = 0;

	UINT16 _hitCount = 0;
	UINT16 _killCount = 0;
	UINT16 _deathCount = 0;
};

