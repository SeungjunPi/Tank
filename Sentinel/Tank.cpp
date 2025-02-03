#include "Tank.h"
#include "Global.h"
#include "GameEvent.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "PlayerManager.h"
#include "ObjectManager.h"
#include "StaticData.h"




Tank::Tank(ObjectID id, UserDBIndex ownerId)
	: GameObject(id, ownerId, true)
{
	
	_mass = TANK_COLLIDER_MASS;
	_radius = TANK_COLLIDER_RADIUS;

	ResetHP();
}

Tank::~Tank()
{
}

void Tank::GetTurretInfo(Transform* out_position, Vector3* out_direction) const
{
	Vector3 position = _transform.Position;
	Quaternion rotation = _transform.Rotation;

	Vector3 v = { 0.0f, -1.0f, 0.0f };
	v = Vector3::Rotate(v, rotation);
	v.x += position.x;
	v.y += position.y;
	v.z += position.z;
	out_position->Position = v;
	out_position->Rotation = _transform.Rotation;

	Vector3 direction = FORWARD_DIRECTION;
	direction = direction * (_radius + PROJECTILE_COLLIDER_RADIUS) * 1.03125f;

	const Vector3 forwardDirection = Vector3::Rotate(direction, rotation);

	memcpy(out_direction, &forwardDirection, sizeof(Vector3));
}

void Tank::ResetHP()
{
	_hp = 5;
}

void Tank::UpdatePlayerInputState(PlayerInputState inputState)
{
	// Hmmm........
	_prevInputState = _crntInputState;
	_crntInputState = inputState;
}

void Tank::PreProcessMovementState()
{
	_translationSpeed = 0.0f;
	_rotationAngle = 0.0f;
	if (_crntInputState & PLAYER_INPUT_FLAG_UP) {
		if (!(_crntInputState & PLAYER_INPUT_FLAG_DOWN)) {
			_translationDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
			_translationSpeed = TANK_TRANSLATION_SPEED;
		}
	}
	else if (_crntInputState & PLAYER_INPUT_FLAG_DOWN) {
		_translationDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation) * -1.f;
		_translationSpeed = TANK_TRANSLATION_SPEED;
	}
	else {
		
	}

	if (_crntInputState & PLAYER_INPUT_FLAG_LEFT) {
		if (!(_crntInputState & PLAYER_INPUT_FLAG_RIGHT)) {
			_rotationAngle = -TANK_ROTATION_SPEED;
		}
	}
	else if (_crntInputState & PLAYER_INPUT_FLAG_RIGHT) {
		_rotationAngle = TANK_ROTATION_SPEED;
	}
	else {
		
	}
}

void Tank::Tick(ULONGLONG tickDiff)
{
	GameObject::Tick(tickDiff);
	if (!IsAlive()) {
		if (g_currentGameTick - _hitTick > TICK_TANK_RESPAWN_INTERVAL) {
			Respawn();
			GamePacket::BroadcastRespawnTank(_id);
		}
	}
	return;
}

BOOL Tank::IsDestroyed(ULONGLONG currentTick) const
{
	return !_isActivatable && (!_isAlive || _hitTick != 0);
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
				_crntInputState = PLAYER_INPUT_NONE;

				g_playerManager.IncreaseDeathCount(_ownerIndex);
				g_playerManager.IncreaseKillCount(other->GetOwnerId());
			}
			g_playerManager.IncreaseHitCount(other->GetOwnerId());
			GamePacket::BroadcastHit(_id, other->GetID());
		}
	}
}

void Tank::OnUpdateTransform()
{
}

void Tank::Respawn()
{
	GameObject::Respawn();
	_hitTick = 0;
	_pCollider->Activate();
	SyncTransformWithCollider();
	ResetHP();
}

BOOL Tank::IsTransformCloseEnough(const Transform* other)
{
	const float TOLERANCE_POSITION_SQUARE = 0.5f; // x, y, z의 차이가 각 1.0 미만인 경우를 간단하게 판단
	const float TOLERANCE_ROTATION = 0.13f; // 15도 미만

	float posDistanceSq = Vector3::DistanceSquared(_transform.Position, other->Position);
	float rotDisctance = Quaternion::AngularDistance(_transform.Rotation, other->Rotation);

	if (posDistanceSq < TOLERANCE_POSITION_SQUARE && rotDisctance < TOLERANCE_ROTATION) {
		// printf("Accept Client's Transform, Diff: [%f, %f]\n", posDistanceSq, rotDisctance);
		return true;
	}
	else {
		// printf("Accept Server's Transform, Diff: [%f, %f]\n", posDistanceSq, rotDisctance);
		return false;
	}
}

BOOL Tank::TryFireMachineGun(ULONGLONG currentTick)
{
	/*if (currentTick < _lastMachineGunFiringTick + TANK_MACHINE_GUN_DELAY) {
		return false;
	}

	Transform projectileTransform;
	Vector3 direction;
	GetTurretInfo(&projectileTransform.Position, &direction);
	projectileTransform.Position = projectileTransform.Position + direction;

	GamePacket::SendFireMachineGun(&projectileTransform);*/
	return true;
}

void Tank::ProcessInput()
{
	PlayerInputState crntMoveStatr = _crntInputState & PLAYER_INPUT_MOVE_FLAGS;
	if (crntMoveStatr) {
		if (_lastTransformSyncTick + TICK_TANK_SYNC < g_currentGameTick) {
			// Send Moving
			// ??
		}
	}
}




