
#include "TankPch.h"

#include <random>

#include "Global.h"
#include "DummyGlobal.h"
#include "KeyboardEventListener.h"
#include "Dummy.h"
#include "Tank.h"
#include "NetCore.h"
#include "DummyPacketHandler.h"
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
	DummyPacketHandler::SendLogin(GetName(), GetPassword(), GetSessionID());
}

void Dummy::OnSuccessLogin(UserDBIndex key, Score score)
{
	Player::OnSuccessLogin(key, score);

	Tank* pObj = g_objectManager.GetTankPtrByOwnerOrNull(key);
	if (pObj != nullptr) {
		SetTank(pObj);
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

	Tank* pTank = GetTankPtr();
	pTank->AdvancePlayerInput(_input);

	PlayerInputState edgeTrigger = _input ^ _prevInput;

	if (edgeTrigger) {
		if (_input == 0) {
			PacketHandler::s_CSSendEndMove(GetTankPtr()->GetTransformPtr(), GetSessionID());
			_lastMovementSyncTick = g_currentGameTick;
			return;
		}

		// Start Move
		PacketHandler::s_CSSendStartMove(GetTankPtr()->GetTransformPtr(), _input, GetSessionID());
		_lastMovementSyncTick = g_currentGameTick;
		return;
	}

	if (_lastMovementSyncTick + TICK_OWN_TANK_SYNC < g_currentGameTick) {
		PacketHandler::s_CSSendMoving(GetTankPtr()->GetTransformPtr(), _input, GetSessionID());
		_lastMovementSyncTick = g_currentGameTick;
	}
}

void Dummy::DetermineInput()
{
	/*if (GetUserID() != 8) {
		printf("sid:%u, uid:%u, input:%u\n", GetSessionID(), GetUserID(), _input);
	}*/

	_prevInput = _input;
	_input = PLAYER_INPUT_NONE; // Reset
	if (_prevReactionTick + DUMMY_REACTION_SPEED > g_currentGameTick) {
		// Reaction이 없으므로 변화 없음.
		_input = _prevInput;
		return;
	}

	Tank* pTank = GetTankPtr();
	Vector3 currentPosition = pTank->GetPosition();
	if (Vector3::DistanceSquared(currentPosition, _destinationCoord) < 1.0f) {
		// 한 프레임 정지, 새로운 방향 설정
		// printf("Reset Destination\n");
		_destinationCoord = GetRandomPlanePosition();
		_input = PLAYER_INPUT_NONE;
		_prevReactionTick = g_currentGameTick;
		return;
	}

	const Transform* pTankTransform = pTank->GetTransformPtr();


	Vector3 forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, pTankTransform->Rotation);


	Vector2 moveDir{ _destinationCoord.x - pTankTransform->Position.x, _destinationCoord.y - pTankTransform->Position.y };
	Vector2 headDir{ forwardDirection.x, forwardDirection.y };

	int moveDetermine = Vector2::DeterminRotationDirection(headDir, moveDir);

	

	if (moveDetermine == 0) {
		_input |= FLAG_PLAYER_INPUT_FORWARD;
		// printf("[GO] FoDir:[%f, %f], MoveDir:[%f, %f]\n", headDir.x, headDir.y, moveDir.x, moveDir.y);
		return;
	}

	if (moveDetermine > 0) {
		// printf("[RR] FoDir:[%f, %f], MoveDir:[%f, %f]\n", headDir.x, headDir.y, moveDir.x, moveDir.y);
		_input |= FLAG_PLAYER_INPUT_ROTATE_RIGHT;
	}
	else {
		// printf("[LL] FoDir:[%f, %f], MoveDir:[%f, %f]\n", headDir.x, headDir.y, moveDir.x, moveDir.y);
		_input |= FLAG_PLAYER_INPUT_ROTATE_LEFT;
	}
}


