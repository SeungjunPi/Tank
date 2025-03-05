

#include "TankPch.h"
#include "Global.h"
#include "Player.h"
#include "Tank.h"

#include "KeyboardEventListener.h"
#include "ClientPacketHandler.h"

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

Tank* Player::GetTankPtr()
{
	return _pTank;
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
	UpdateInputState(heldKeys);
	
	if (!_pTank->IsAlive()) {
		return;
	}
	
	_pTank->AdvancePlayerInput(_crntInputState);

	// Fire Machine Gun
	if (_crntInputState & FLAG_PLAYER_INPUT_FIRE_MACHINE_GUN) {
		BOOL fired = _pTank->TryFireMachineGun(g_currentGameTick);
		if (fired) {
			PacketHandler::s_CSSendFireMachineGun(nullptr, _sessionID);
		}
	}

	// Edge
	PlayerInputState prevMoveState = _prevInputState & FLAGS_PLAYER_INPUT_MOVEMENT;
	PlayerInputState crntMoveState = _crntInputState & FLAGS_PLAYER_INPUT_MOVEMENT;
	PlayerInputState moveEdgeTrigger = prevMoveState ^ crntMoveState;
	if (crntMoveState == 0) {
		if (moveEdgeTrigger) {
			PacketHandler::s_CSSendEndMove(_pTank->GetTransformPtr(), GetSessionID());
			_lastMovementSyncTick = g_currentGameTick;
			return;
		}
		return;
	}

	if (prevMoveState == 0) {
		// Start Move
		PacketHandler::s_CSSendStartMove(_pTank->GetTransformPtr(), crntMoveState, GetSessionID());
		_lastMovementSyncTick = g_currentGameTick;
		return;
	}

	if (_lastMovementSyncTick + TICK_OWN_TANK_SYNC < g_currentGameTick) {
		PacketHandler::s_CSSendMoving(_pTank->GetTransformPtr(), crntMoveState, g_pPlayer->GetSessionID());
		_lastMovementSyncTick = g_currentGameTick;
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

void Player::Tick()
{
	HandleKeyboardEvents(g_keyboardPressedFlag, g_keyboardReleasedFlag, g_keyboardHeldFlag);

	// Send Keyboard Event
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

void Player::UpdateInputState(UINT64 heldKeys)
{
	_prevInputState = _crntInputState;
	_crntInputState = PLAYER_INPUT_NONE;
	if (heldKeys & KEYBOARD_INPUT_FLAG_SPACE) {
		_crntInputState |= FLAG_PLAYER_INPUT_FIRE_MACHINE_GUN;
	}
	
	if (heldKeys & KEYBOARD_INPUT_FLAG_UP) {
		_crntInputState |= FLAG_PLAYER_INPUT_FORWARD;
	}
	
	if (heldKeys & KEYBOARD_INPUT_FLAG_DOWN) {
		_crntInputState |= FLAG_PLAYER_INPUT_BACKWARD;
	}
	
	if (heldKeys & KEYBOARD_INPUT_FLAG_LEFT) {
		_crntInputState |= FLAG_PLAYER_INPUT_ROTATE_LEFT;
	}
	
	if (heldKeys & KEYBOARD_INPUT_FLAG_RIGHT) {
		_crntInputState |= FLAG_PLAYER_INPUT_ROTATE_RIGHT;
	}
}


