#pragma once

#include "SentinelPch.h"
#include "SentinelAliases.h"
#include "StaticData.h"

class Tank;
class PlayerManager;

class Player
{
	friend PlayerManager;
public:
	Player(SessionID sessionID, UserDBIndex userID);
	SessionID GetSessionID() const;

	UserDBIndex GetUserIndex() const { return _userIndex; }

	Tank* GetTank() const { return _pTank; }
	void SetTank(Tank* pTank) { _pTank = pTank; }

	TankScore GetScore() const;

	void IncreaseHitCount();
	void IncreaseKillCount();
	void IncreaseDeathCount();



	void TryFireMachineGun();

private:
	SessionID _sessionId = 0;
	UINT32 _userIndex = 0;
	
	Tank* _pTank = nullptr;

	UINT16 _hitCount = 0;
	UINT16 _killCount = 0;
	UINT16 _deathCount = 0;
};

