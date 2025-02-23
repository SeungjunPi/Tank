
#include "DummyPch.h"

#include <random>

#include "Global.h"
#include "KeyboardEventListener.h"
#include "Dummy.h"
#include "Tank.h"
#include "NetCore.h"
#include "GameEvent.h"
#include "AllocObjectManager.h"

static Vector3 GetRandomPlanePosition()
{
	static std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(-100, 100);

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

	GameObject* pObj = g_objectManager.GetObjectPtrByOwnerOrNull(key);
	if (pObj != nullptr) {
		SetTank((Tank*)pObj);
	}
}

SessionID Dummy::ConnectToServer()
{
	SessionID serverID;
	if (!_isConnecting) {
		serverID = g_pNetCore->ConnectTo("127.0.0.1", 30283);
		OnConnected(serverID);
		_isConnecting = true;
		return serverID;
	}
	return GetSessionID();
}

void Dummy::Tick()
{
	if (!_isConnecting) {
		return;
	}

	ObjectID objID = GetTankID();
	if (objID.equals(INVALID_OBJECT_ID)) {
		return;
	}

	DetermineInput();

	Tank* pTank = GetTank();
	pTank->UpdatePlayerInputState(_input);

	UINT64 edgeTrigger = _input ^ _prevInput;
	if (edgeTrigger) {
		// 변화가 있다는건 Reaction이 생겼다는 뜻.
		_prevReactionTick = g_currentGameTick;
	}
	_prevInput = _input;
}

void Dummy::DetermineInput()
{
	_input = 0; // Reset
	if (_prevReactionTick + DUMMY_REACTION_SPEED > g_currentGameTick) {
		// Reaction이 없으므로 변화 없음.
		_input = _prevInput;
		return;
	}

	Tank* pTank = GetTank();
	Vector3 currentPosition = pTank->GetPosition();
	if (Vector3::DistanceSquared(currentPosition, _destinationCoord) < 1.0f) {
		// 한 프레임 정지, 새로운 방향 설정
		// printf("Reset Destination\n");
		_destinationCoord = GetRandomPlanePosition();
		_input = 0;
		return;
	}
	
	const Transform* pTankTransform = pTank->GetTransformPtr();
	
	
	Vector3 forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, pTankTransform->Rotation);


	Vector2 moveDir{ _destinationCoord.x - pTankTransform->Position.x, _destinationCoord.y - pTankTransform->Position.y };
	Vector2 headDir{ forwardDirection.x, forwardDirection.y };

	int moveDetermine = Vector2::DeterminRotationDirection(headDir, moveDir);

	if (moveDetermine == 0) {
		_input |= KEYBOARD_INPUT_FLAG_UP;
		// printf("[GO] FoDir:[%f, %f], MoveDir:[%f, %f]\n", headDir.x, headDir.y, moveDir.x, moveDir.y);
		return;
	}

	if (moveDetermine > 0) {
		// printf("[RR] FoDir:[%f, %f], MoveDir:[%f, %f]\n", headDir.x, headDir.y, moveDir.x, moveDir.y);
		_input |= KEYBOARD_INPUT_FLAG_RIGHT;
	}
	else {
		// printf("[LL] FoDir:[%f, %f], MoveDir:[%f, %f]\n", headDir.x, headDir.y, moveDir.x, moveDir.y);
		_input |= KEYBOARD_INPUT_FLAG_LEFT;
	}
}


