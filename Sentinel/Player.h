#pragma once

#include "SentinelPch.h"
#include "SentinelAliases.h"

struct TankScore
{
	UINT16 hitCount = 0;
	UINT16 killCount = 0;
	UINT16 deathCount = 0;
};

class PlayerManager;

class Player
{
	friend PlayerManager;
public:
	Player(SessionID sessionID, UserDBIndex userID);
	SessionID GetSessionID() const;

	UserDBIndex GetUserIndex() const { return _userIndex; }

	UINT32 GetTankId() const;
	void SetTankId(UINT32 tankId);

	TankScore GetScore() const;

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

