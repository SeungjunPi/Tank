#pragma once

#include "stdafx.h"

class Player
{
public:
	Player(UINT32 sessionId);
	UINT32 GetSessionId() const;

	UINT16 GetTankId() const;
	void SetTankId(UINT16 tankId);

	void IncreaseNumOtherTanksHit();
	
	void IncreaseNumHitsTaken();

	UINT16 GetScore() const;
	
private:
	UINT32 _sessionId = 0;
	UINT16 _tankId = 0;

	UINT16 _score = 0;
	UINT16 _numOtherTanksHit = 0;
	UINT16 _numHitsTaken = 0;
};

