#include "Tank.h"
#include "StaticData.h"
#include "Global.h"
#include "GameEvent.h"
#include "Collider.h"
#include "ICollisionManager.h"
#include "AllocObjectManager.h"
#include "Camera.h"
#include "Player.h"

Tank::Tank(ObjectID id, UserDBIndex ownerID)
	: GameObject(id, ownerID, true)
{	
	_model = g_pTankModel;

	ResetHP();
}

Tank::~Tank()
{
}

void Tank::GetTurretInfo(Vector3* out_position, Vector3* out_direction) const
{
	Vector3 position = _transform.Position;
	Quaternion rotation = _transform.Rotation;

	Vector3 v = _model.vertices[0].v;
	v = Vector3::Rotate(v, rotation);
	v.x += position.x;
	v.y += position.y;
	v.z += position.z;
	*out_position = v;

	Vector3 direction = FORWARD_DIRECTION;
	direction = direction * (_radius + PROJECTILE_COLLIDER_RADIUS) * 1.03125f;

	const Vector3 forwardDirection = Vector3::Rotate(direction, rotation);

	memcpy(out_direction, &forwardDirection, sizeof(Vector3));
}

void Tank::ResetHP()
{
	_hp = 5;
}

void Tank::UpdatePlayerInputStateFromServer(PlayerInputState inputState)
{
	if (_ownerID == g_pPlayer->GetUserID()) {
		return;
	}
	UpdatePlayerInputState(inputState);
}

void Tank::UpdatePlayerInputState(PlayerInputState inputState)
{
	_previousInputState = _currentInputState;
	_currentInputState = inputState;
}

BOOL Tank::IsDestroyed(ULONGLONG currentTick) const
{
	return !_isActivatable && (!_isAlive || _hitTick != 0);
}

void Tank::PreProcessMovementState()
{
	_translationSpeed = 0.0f;
	_rotationAngle = 0.0f;
	if (_currentInputState & PLAYER_INPUT_FLAG_UP) {
		if (!(_currentInputState & PLAYER_INPUT_FLAG_DOWN)) {
			_translationDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
			_translationSpeed = TANK_TRANSLATION_SPEED;
		}
	}
	else if (_currentInputState & PLAYER_INPUT_FLAG_DOWN) {
		_translationDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation) * -1.f;
		_translationSpeed = TANK_TRANSLATION_SPEED;
	}
	else {
		
	}

	_rotationAngle = 0.0f;
	if (_currentInputState & PLAYER_INPUT_FLAG_LEFT) {
		if (!(_currentInputState & PLAYER_INPUT_FLAG_RIGHT)) {
			_rotationAngle = -TANK_ROTATION_SPEED;
		}
	}
	else if (_currentInputState & PLAYER_INPUT_FLAG_RIGHT) {
		_rotationAngle = TANK_ROTATION_SPEED;
	}
	else {
		
	}
}

void Tank::Tick(ULONGLONG tickDiff)
{
	GameObject::Tick(tickDiff);

	
	ProcessInput();

	return;
}

void Tank::OnHitWith(ULONGLONG currentTick, GameObject* other)
{
	ObjectID otherID = other->GetID();
	if (otherID.type == GAME_OBJECT_TYPE_PROJECTILE) {
		if (_hp > 0) {
			if (--_hp == 0) {
				_pCollider->Deactivate();
				_hitTick = currentTick;
				_isAlive = false;
				_currentInputState = PLAYER_INPUT_NONE;

				if (_id.equals(g_pPlayer->GetTankID())) {
					g_pPlayer->IncreaseDeath();
				}

				if (other->GetID().equals(g_pPlayer->GetTankID())) {
					g_pPlayer->IncreaseKill();
				}
			}
		}
	}
}

void Tank::OnUpdateTransform()
{
	if (g_pPlayer->GetUserID() == _ownerID) {
		//g_pCamera->UpdateTransf(&_transform);
	}
}

void Tank::OnRespawn()
{
	GameObject::OnRespawn();
	_hitTick = 0;
	_pCollider->Activate();
	ResetHP();
}

BOOL Tank::TryFireMachineGun(ULONGLONG currentTick)
{
	if (currentTick < _lastMachineGunFiringTick + TANK_MACHINE_GUN_DELAY) {
		return false;
	}

	Transform projectileTransform;
	Vector3 direction;
	GetTurretInfo(&projectileTransform.Position, &direction);
	projectileTransform.Position = projectileTransform.Position + direction;
	
	GamePacket::SendFireMachineGun(&projectileTransform);
	return true;
}

void Tank::ProcessInput()
{
	if (_ownerID != g_pPlayer->GetUserID()) {
		return;
	}
	// Send Fire
	if (_currentInputState & PLAYER_INPUT_FLAG_FIRE_MACHINE_GUN) {
		BOOL fired = TryFireMachineGun(g_currentGameTick);
		if (fired) {
			_lastMachineGunFiringTick = g_currentGameTick;
		}
	}

	PlayerInputState prevMoveState = _previousInputState & PLAYER_INPUT_MOVE_FLAGS;
	PlayerInputState crntMoveState = _currentInputState & PLAYER_INPUT_MOVE_FLAGS;
	PlayerInputState moveEdgeTrigger = prevMoveState ^ crntMoveState;
	if (crntMoveState == 0) {
		if (moveEdgeTrigger) {
			GamePacket::SendEndMove(&_transform);
			_lastTransformSyncTick = g_currentGameTick;
			return;
		}
		return;
	}

	if (prevMoveState == 0) {
		// Start Move
		GamePacket::SendStartMove(&_transform, crntMoveState);
		_lastTransformSyncTick = g_currentGameTick;
		return;
	}

	if (_lastTransformSyncTick + TICK_OWN_TANK_SYNC < g_currentGameTick) {
		GamePacket::SendMoving(&_transform, crntMoveState);
		_lastTransformSyncTick = g_currentGameTick;
	}
}


