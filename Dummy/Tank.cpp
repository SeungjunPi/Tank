#include "Tank.h"
#include "StaticData.h"
#include "Global.h"
#include "GameEvent.h"
#include "Collider.h"
#include "IStableFlow.h"
#include "AllocObjectManager.h"
#include "Camera.h"
#include "Player.h"
#include "DummyManager.h"
#include "Dummy.h"

Tank::Tank(ObjectID id, UserDBIndex ownerID)
	: GameObject(id, ownerID, true)
{
	_model = g_pTankModel;
	_physicalComponent.mass = TANK_COLLIDER_MASS;
	_physicalComponent.radius = TANK_COLLIDER_RADIUS;
	_physicalComponent.colliderType = COLLIDER_TYPE_SPHERE;

	ResetHP();
}

Tank::~Tank()
{
}

void Tank::GetTurretInfo(Transform* out_position, Vector3* out_direction) const
{
	Vector3 position = _physicalComponent.transform.Position;
	Quaternion rotation = _physicalComponent.transform.Rotation;

	Vector3 v = _model.vertices[0].v;
	v = Vector3::Rotate(v, rotation);
	v.x += position.x;
	v.y += position.y;
	v.z += position.z;
	out_position->Position = v;
	out_position->Rotation = _physicalComponent.transform.Rotation;

	Vector3 direction = FORWARD_DIRECTION;
	direction = direction * (_physicalComponent.radius + PROJECTILE_COLLIDER_RADIUS) * 1.03125f;

	const Vector3 forwardDirection = Vector3::Rotate(direction, rotation);

	memcpy(out_direction, &forwardDirection, sizeof(Vector3));
}

void Tank::ResetHP()
{
	_hp = 5;
}

void Tank::UpdatePlayerInputStateFromServer(PlayerInputState inputState)
{
	AdvancePlayerInput(inputState);
}

void Tank::AdvancePlayerInput(PlayerInputState inputState)
{
	_prevInputState = _crntInputState;
	_crntInputState = inputState;

	if ((_crntInputState & PLAYER_INPUT_FLAG_UP) && !(_crntInputState & PLAYER_INPUT_FLAG_DOWN)) {
		_physicalComponent.velocity = Vector3::Rotate(FORWARD_DIRECTION, _physicalComponent.transform.Rotation) * TANK_TRANSLATION_SPEED;
	}
	else if ((_crntInputState & PLAYER_INPUT_FLAG_DOWN) && !(_crntInputState & PLAYER_INPUT_FLAG_UP)) {
		_physicalComponent.velocity = Vector3::Rotate(BACKWARD_DIRECTION, _physicalComponent.transform.Rotation) * TANK_TRANSLATION_SPEED;
	}
	else {
		_physicalComponent.velocity = Vector3();
	}

	if (_crntInputState & PLAYER_INPUT_FLAG_LEFT) {
		if (!(_crntInputState & PLAYER_INPUT_FLAG_RIGHT)) {
			_physicalComponent.angularVelocity = { 0, 0, -TANK_ROTATION_SPEED };
		}
		else {
			_physicalComponent.angularVelocity = Vector3();
		}
	}
	else if (_crntInputState & PLAYER_INPUT_FLAG_RIGHT) {
		_physicalComponent.angularVelocity = { 0, 0, TANK_ROTATION_SPEED };
	}
	else {
		_physicalComponent.angularVelocity = Vector3();
	}
}

BOOL Tank::IsDestroyed(ULONGLONG currentTick) const
{
	return !_isActivatable && (!_isAlive || _hitTick != 0);
}

void Tank::Tick(ULONGLONG tickDiff)
{
	GameObject::Tick(tickDiff);


	ProcessInput();

	return;
}

void Tank::OnHitWith(ULONGLONG currentTick, GameObject* other)
{

}

void Tank::OnUpdateTransform()
{
}

void Tank::OnRespawn()
{
	GameObject::OnRespawn();
	_hitTick = 0;
	_pCollider->Activate();
	ResetHP();
}

void Tank::OnHitServer(ULONGLONG currentTick, GameObject* other)
{
	ObjectID otherID = other->GetID();
	Dummy* pOwner = g_pDummyManager->GetDummyByUserID(_ownerID);
	Dummy* pHitter = g_pDummyManager->GetDummyByUserID(other->GetOwnerID());
	if (otherID.type == GAME_OBJECT_TYPE_PROJECTILE) {
		if (_hp > 0) {
			pHitter->IncreaseHit();
			if (--_hp == 0) {
				_pCollider->Deactivate();
				_hitTick = currentTick;
				_isAlive = false;
				_crntInputState = PLAYER_INPUT_NONE;

				
				pOwner->IncreaseDeath();
				pHitter->IncreaseKill();
			}
		}
	}
}

BOOL Tank::TryFireMachineGun(ULONGLONG currentTick)
{
	if (currentTick < _lastMachineGunFiringTick + TANK_MACHINE_GUN_DELAY) {
		return false;
	}

	Transform projectileTransform;
	Vector3 direction;
	GetTurretInfo(&projectileTransform, &direction);

	// 자신이 맞지 않기 위해 여유분을 1.0625만큼 줌
	projectileTransform.Position = projectileTransform.Position + direction * 1.0625f;
	Dummy* pOwner = g_pDummyManager->GetDummyByUserID(_ownerID);
	GamePacket::SendFireMachineGun(&projectileTransform, pOwner->GetSessionID());
	return true;
}

void Tank::ProcessInput()
{
	if (_crntInputState & PLAYER_INPUT_FLAG_FIRE_MACHINE_GUN) {
		BOOL fired = TryFireMachineGun(g_currentGameTick);
		if (fired) {
			_lastMachineGunFiringTick = g_currentGameTick;
		}
	}

	Dummy* pOwner = g_pDummyManager->GetDummyByUserID(_ownerID);
	if (pOwner == nullptr) {
		return;
	}

	PlayerInputState prevMoveState = _prevInputState & PLAYER_INPUT_MOVE_FLAGS;
	PlayerInputState crntMoveState = _crntInputState & PLAYER_INPUT_MOVE_FLAGS;
	PlayerInputState moveEdgeTrigger = prevMoveState ^ crntMoveState;
	if (crntMoveState == 0) {
		if (moveEdgeTrigger) {
			GamePacket::SendEndMove(&_physicalComponent.transform, pOwner->GetSessionID());
			_lastTransformSyncTick = g_currentGameTick;
			return;
		}
		return;
	}

	if (prevMoveState == 0) {
		// Start Move
		GamePacket::SendStartMove(&_physicalComponent.transform, crntMoveState, pOwner->GetSessionID());
		_lastTransformSyncTick = g_currentGameTick;
		return;
	}

	if (_lastTransformSyncTick + TICK_OWN_TANK_SYNC < g_currentGameTick) {
		GamePacket::SendMoving(&_physicalComponent.transform, crntMoveState, pOwner->GetSessionID());
		_lastTransformSyncTick = g_currentGameTick;
	}
}

