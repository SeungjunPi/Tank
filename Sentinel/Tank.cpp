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
	_movementSpeed = TANK_MOVEMENT_SPEED;
	_rotationSpeed = TANK_ROTATION_SPEED;
	_mass = TANK_COLLIDER_MASS;
	_radius = TANK_COLLIDER_RADIUS;

	ResetHP();
}

Tank::~Tank()
{
}

void Tank::StartMove(EMOVEMENT movement)
{
	switch (movement) {
	case EMOVEMENT::FORWARD:
		_isMovingFoward = true;
		break;
	case EMOVEMENT::BACKWARD:
		_isMovingBackward = true;
		break;
	default:
		__debugbreak();
	}
}

void Tank::EndMove(EMOVEMENT movement)
{
	switch (movement) {
	case EMOVEMENT::FORWARD:
		_isMovingFoward = false;
		break;
	case EMOVEMENT::BACKWARD:
		_isMovingBackward = false;
		break;
	default:
		__debugbreak();
	}
}

void Tank::StartRotate(EROTATION rotation)
{
	switch (rotation) {
	case EROTATION::LEFT:
		_isRotatingLeft = true;
		break;
	case EROTATION::RIGHT:
		_isRotatingRight = true;
		break;
	default:
		__debugbreak();
	}
}

void Tank::EndRotate(EROTATION rotation)
{
	switch (rotation) {
	case EROTATION::LEFT:
		_isRotatingLeft = false;
		break;
	case EROTATION::RIGHT:
		_isRotatingRight = false;
		break;
	default:
		__debugbreak();
	}
}
void Tank::GetTurretInfo(Vector3* out_position, Vector3* out_direction) const
{
	Vector3 position = _transform.Position;
	Quaternion rotation = _transform.Rotation;

	Vector3 v = { 0, -1.f, 0 };
	v = Vector3::Rotate(v, rotation);
	v.x += position.x;
	v.y += position.y;
	v.z += position.z;
	*out_position = v;

	const Vector3 forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);

	memcpy(out_direction, &forwardDirection, sizeof(Vector3));
}

void Tank::ResetHP()
{
	_hp = 5;
}

void Tank::OnFrame(ULONGLONG tickDiff)
{
	if (g_currentGameTick - _hitTick > TICK_TANK_RESPAWN_INTERVAL) {
		OnRespawn();
		GamePacket::BroadcastRespawnTank(_id);
	}
	return;
}

Vector3 Tank::GetMovementVector(ULONGLONG tickDiff)
{
	if (!(_isMovingFoward ^ _isMovingBackward)) {
		return { 0, 0, 0 };
	}
	
	Vector3 movement = GameObject::GetMovementVector(tickDiff);
	if (_isMovingBackward) {
		return movement * -1;
	}

	return movement;
}

Quaternion Tank::GetDeltaRotation(ULONGLONG tickDiff)
{
	if (!(_isRotatingLeft ^ _isRotatingRight)) {
		return { 1.0f, 0.0f, 0.0f, 0.0f };
	}

	if (_isRotatingLeft) {
		return Quaternion::RotateZM(_rotationSpeed * tickDiff, _rotationDirection);
	}

	return Quaternion::RotateZP(_rotationSpeed * tickDiff, _rotationDirection);
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
				_isMovingFoward = false;
				_isMovingBackward = false;
				_isRotatingLeft = false;
				_isRotatingRight = false;
				g_playerManager.IncreaseDeathCount(_ownerIndex);
				g_playerManager.IncreaseKillCount(other->GetOwnerId());
			}
			GamePacket::BroadcastTankHit(_id, other->GetID(), other->GetOwnerId(), _ownerIndex);
		}
	}
}

void Tank::OnRespawn()
{
	GameObject::OnRespawn();
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
		printf("Accept Client's Transform, Diff: [%f, %f]\n", posDistanceSq, rotDisctance);
		return true;
	}
	else {
		printf("Accept Server's Transform, Diff: [%f, %f]\n", posDistanceSq, rotDisctance);
		return false;
	}
}




