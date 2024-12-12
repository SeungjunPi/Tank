#include "Player.h"

Player::Player(UINT32 sessionId, UINT32 userID)
	: _sessionId(sessionId)
	, _userID(userID)
{
}

UINT32 Player::GetSessionID() const
{
	return _sessionId;
}

UINT16 Player::GetTankId() const
{
	return _tankId;
}

void Player::SetTankId(UINT16 tankId)
{
	_tankId = tankId;
}

void Player::IncreaseNumOtherTanksHit()
{
	++_numOtherTanksHit;
}

void Player::IncreaseNumHitsTaken()
{
	++_numHitsTaken;
}

UINT16 Player::GetScore() const
{
	
	if (_numOtherTanksHit * 10 < _numHitsTaken) {
		return 0;
	}
	return _numOtherTanksHit * 10 - _numHitsTaken;;
}
