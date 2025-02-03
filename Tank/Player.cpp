

#include "TankPch.h"
#include "Global.h"
#include "Player.h"
#include "Tank.h"

#include "KeyboardEventListener.h"
#include "GameEvent.h"

Player::Player(std::wstring name, std::wstring password)
	: _name(name)
	, _password(password)
	, _score({ 0, 0, 0 })
{
}

Player::~Player()
{
}

void Player::OnConnected(SessionID sessionID)
{
	if (_sessionID != INVALID_SESSION_ID) {
		__debugbreak();
		return;
	}
	_sessionID = sessionID;
}

void Player::OnSuccessLogin(UserDBIndex key, Score score)
{
	if (_userID != INVALID_USER_ID) {
		__debugbreak();
		return;
	}
	_userID = key;
	memcpy(&_score, &score, sizeof(Score));
	memcpy(&g_score, &_score, sizeof(Score));
}

void Player::SetTank(Tank* pTank)
{
	if (_pTank != nullptr) {
		__debugbreak();
	}
	_pTank = pTank;
}

ObjectID Player::GetTankID()
{
	if (_pTank) {
		return _pTank->GetID();
	}
	
	return INVALID_OBJECT_ID;
}

Tank* Player::ClearTank()
{
	if (_pTank == nullptr) {
		__debugbreak();
	}
	
	Tank* tmp = _pTank;
	_pTank = nullptr;
	return tmp;
}

void Player::HandleKeyboardEvents(UINT64 pressedKeys, UINT64 releasedKeys, UINT64 heldKeys)
{
	if (_pTank == nullptr) {
		// Do something?
		return;
	}

	// Convert KeyboardInput to PlayerInput
	
	_pTank->UpdatePlayerInputState(heldKeys);
}

INT Player::IncreaseHit()
{
	g_score.hit = ++_score.hit;
	return _score.hit;
}

INT Player::IncreaseKill()
{
	g_score.kill = ++_score.kill;
	return _score.kill;
}

INT Player::IncreaseDeath()
{
	g_score.death = ++_score.death;
	return _score.death;
}

void Player::LogTankPosition(const char* str)
{
	if (_pTank == nullptr) {
		printf("No Tank\n");
		return;
	}

	Vector3 pos = _pTank->GetPosition();

	printf("[%s], [%f, %f, %f]\n", str, pos.x, pos.y, pos.z);
}


