
#include "TankPch.h"

#include <random>

#include "Global.h"
#include "KeyboardEventListener.h"
#include "Dummy.h"
#include "Tank.h"
#include "NetCore.h"
#include "GameEvent.h"

static Vector3 GetRandomPlanePosition()
{
	static std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(-15, 15);

	Vector3 pos{ dist(gen), dist(gen), 0.0 };

	return pos;
}


Dummy::Dummy(std::wstring name, std::wstring pw)
	: Player(name, pw)
{
}

void Dummy::OnConnected(SessionID sessionID)
{
	Player::OnConnected(sessionID);

	// Login
	GamePacket::SendLogin(GetName(), GetPassword(), GetSessionID());
}

void Dummy::OnSuccessLogin(UserDBIndex key, Score score)
{
	Player::OnSuccessLogin(key, score);
}

void Dummy::OnFrame()
{
	if (!_isConnecting) {
		SessionID serverID = g_pNetCore->ConnectTo("127.0.0.1", 30283);
		OnConnected(serverID);
		_isConnecting = true;
		return;
	}

	ObjectID objID = GetTankID();
	if (objID.equals(INVALID_OBJECT_ID)) {
		return;
	}

	Tank* pTank = GetTank();
	Vector3 currentPosition = pTank->GetPosition();
	if (Vector3::DistanceSquared(currentPosition, _destinationCoord) < 1.0f) {
		// 한 프레임 정지, 새로운 방향 설정
		printf("Reset Destination\n");
		_destinationCoord = GetRandomPlanePosition();
	}
	else {
		// forward가 destination으로 충분히 향하고 있다면 전진만, 아니면 회전만 함. 
		DetermineInput();
	}
	
	UINT64 edgeTrigger = _virtualInputEvent ^ _prevInputEvent;
	if (edgeTrigger) {
		// 사람의 반응속도 이상을 할 수 없으므로..
		if (_prevTick + DUMMY_TICK_INTERVAL > g_currentGameTick) {
			return;
		}
		_prevTick = g_currentGameTick;
	}
	UINT64 pressedKeys = edgeTrigger & _virtualInputEvent;
	UINT64 releasedKeys = edgeTrigger & (~_virtualInputEvent);
	if (releasedKeys || pressedKeys) {
		Vector3 moveDir = pTank->GetForwardDirection();
		Vector3 dstDir = _destinationCoord;
		dstDir.x -= pTank->GetPosition().x;
		dstDir.y -= pTank->GetPosition().y;

		float sq = dstDir.x * dstDir.x + dstDir.y * dstDir.y;

		dstDir.x /= sq;
		dstDir.y /= sq;

		//printf("DstDir[%f, %f], MoveDir[%f, %f], %X, %X\n", dstDir.x, dstDir.y, moveDir.x, moveDir.y,  pressedKeys, releasedKeys);
	}
	

	HandleKeyboardEvents(pressedKeys, releasedKeys, _virtualInputEvent);

	_prevInputEvent = _virtualInputEvent;
	_virtualInputEvent = 0;
}

void Dummy::DetermineInput()
{
	Tank* pTank = GetTank();
	Vector3 tankPosition = pTank->GetPosition();
	Vector3 forwardDirection = pTank->GetForwardDirection();

	Vector2 moveDir{ _destinationCoord.x - tankPosition.x, _destinationCoord.y - tankPosition.y };
	Vector2 headDir{ forwardDirection.x, forwardDirection.y };

	int moveDetermine = Vector2::DeterminRotationDirection(headDir, moveDir);

	if (moveDetermine == 0) {
		_virtualInputEvent |= KEYBOARD_INPUT_FLAG_UP;
		printf("[GO] FoDir:[%f, %f], MoveDir:[%f, %f]\n", headDir.x, headDir.y, moveDir.x, moveDir.y);
		return;
	}

	if (moveDetermine > 0) {
		printf("[RR] FoDir:[%f, %f], MoveDir:[%f, %f]\n", headDir.x, headDir.y, moveDir.x, moveDir.y);
		_virtualInputEvent |= KEYBOARD_INPUT_FLAG_RIGHT;
	}
	else {
		printf("[LL] FoDir:[%f, %f], MoveDir:[%f, %f]\n", headDir.x, headDir.y, moveDir.x, moveDir.y);
		_virtualInputEvent |= KEYBOARD_INPUT_FLAG_LEFT;
	}
}


