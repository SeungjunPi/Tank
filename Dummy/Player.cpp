

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

	if (pressedKeys & KEYBOARD_INPUT_FLAG_UP) {
		_pTank->StartMove(EMovement::FORWARD);
	}

	if (pressedKeys & KEYBOARD_INPUT_FLAG_DOWN) {
		_pTank->StartMove(EMovement::BACKWARD);
	}
	if (pressedKeys & KEYBOARD_INPUT_FLAG_LEFT) {
		_pTank->StartRotate(ERotation::LEFT);
	}
				
	if (pressedKeys & KEYBOARD_INPUT_FLAG_RIGHT) {
		_pTank->StartRotate(ERotation::RIGHT);
	}

	if (pressedKeys & 0xFF) {
		char moveFlag = pressedKeys & 0xFF;
		GamePacket::SendStartMove(_pTank->GetTransformPtr(), moveFlag, _sessionID);
		//g_lastOwnTankSyncTick = g_currentGameTick;
	}


	if (releasedKeys & KEYBOARD_INPUT_FLAG_UP) {
		_pTank->EndMove(EMovement::FORWARD);
	}

	if (releasedKeys & KEYBOARD_INPUT_FLAG_DOWN) {
		_pTank->EndMove(EMovement::BACKWARD);
	}

	if (releasedKeys & KEYBOARD_INPUT_FLAG_LEFT) {
		_pTank->EndRotate(ERotation::LEFT);
	}

	if (releasedKeys & KEYBOARD_INPUT_FLAG_RIGHT) {
		_pTank->EndRotate(ERotation::RIGHT);
	}

	if (releasedKeys & 0xFF) {
		char moveFlag = releasedKeys & 0xFF;
		GamePacket::SendEndMove(_pTank->GetTransformPtr(), moveFlag, _sessionID);
	}

	if (heldKeys & KEYBOARD_INPUT_FLAG_SPACE) {
		// 서버에서 오기 전 반복해서 Fire 요청을 보낼 수 있으나, 서버에서 무시될 것.
		bool canFire = _pTank->CanFireMachineGun();
		if (canFire) {
			Transform transf = { 0.f, };
			_pTank->GetTurretInfo(&transf);
			GamePacket::SendFireMachineGun(&transf, _sessionID);
		}
	}
	
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

