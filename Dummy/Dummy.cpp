
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
		if (!RotateTankIfNotHeadToDest()) {
			_virtualInputEvent |= KEYBOARD_INPUT_FLAG_UP;
		}
	}
	
	UINT64 edgeTrigger = _virtualInputEvent ^ _prevInputEvent;
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

		printf("DstDir[%f, %f], MoveDir[%f, %f], %X, %X\n", dstDir.x, dstDir.y, moveDir.x, moveDir.y,  pressedKeys, releasedKeys);
	}
	

	HandleKeyboardEvents(pressedKeys, releasedKeys, _virtualInputEvent);

	_prevInputEvent = _virtualInputEvent;
	_virtualInputEvent = 0;
}

bool Dummy::RotateTankIfNotHeadToDest()
{
	Tank* pTank = GetTank();
	Vector3 tankPosition = pTank->GetPosition();
	Vector3 forwardDirection = pTank->GetForwardDirection();

	Vector3 v1{ _destinationCoord.x - tankPosition.x, _destinationCoord.y - tankPosition.y, _destinationCoord.z - tankPosition.z };
	float norm = sqrtf(v1.x * v1.x + v1.y * v1.y);
	v1.x /= norm;
	v1.y /= norm;
	

	float cosAngle = (v1.x * forwardDirection.x + v1.y * forwardDirection.y);
	printf("FoDir:[%f, %f], DstDir:[%f, %f]\n", forwardDirection.x, forwardDirection.y, v1.x, v1.y);
	if (cosAngle > 0.75) {
		// Do not rotate
		return false;
	}
	
	float cross = v1.y * forwardDirection.x - v1.x * forwardDirection.y;
	if (cross <= 0) {
		// Rotate left
		_virtualInputEvent |= KEYBOARD_INPUT_FLAG_LEFT;
	}
	else {
		// Rotate right
		_virtualInputEvent |= KEYBOARD_INPUT_FLAG_RIGHT;
	}
	return true;
}


