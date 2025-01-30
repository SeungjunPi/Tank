#include "Player.h"
#include "Global.h"
#include "Tank.h"

Player::Player(SessionID sessionId, UserDBIndex userID)
	: _sessionId(sessionId)
	, _userIndex(userID)
{
}

SessionID Player::GetSessionID() const
{
	return _sessionId;
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

void Player::TryFireMachineGun()
{
	if (_pTank != nullptr) {
		// Fire Machine Gun
	}
}

