#pragma once

#include "stdafx.h"

class Player
{
public:
	Player(UINT32 sessionID, UINT32 userID);
	UINT32 GetSessionID() const;

	UINT32 GetUserID() const { return _userID; }

	UINT16 GetTankId() const;
	void SetTankId(UINT16 tankId);

	void IncreaseNumOtherTanksHit();
	
	void IncreaseNumHitsTaken();

	UINT16 GetScore() const;
	
private:
	UINT32 _sessionId = 0;
	UINT32 _userID = 0;
	UINT16 _tankId = 0;

	UINT16 _score = 0;
	UINT16 _numOtherTanksHit = 0;
	UINT16 _numHitsTaken = 0;
};

