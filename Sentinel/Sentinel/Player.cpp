#include "Player.h"

Player::Player(SessionID sessionId, UserDBIndex userID)
	: _sessionId(sessionId)
	, _userIndex(userID)
{
}

SessionID Player::GetSessionID() const
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

TankScore Player::GetScore() const
{
	return TankScore{ _hitCount, _killCount, _deathCount };
}

void Player::IncreaseHitCount()
{
	++_hitCount;
}

void Player::IncreaseKillCount()
{
	++_killCount;
}

void Player::IncreaseDeathCount()
{
	++_deathCount;
}

