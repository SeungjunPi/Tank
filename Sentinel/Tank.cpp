#include "Tank.h"
#include "Global.h"
#include "GameEvent.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "PlayerManager.h"
#include "ObjectManager.h"
#include "StaticData.h"

const float POSITION_VELOCITY_WEIGHT = 0.5f;
const float ROTATION_VELOCITY_WEIGHT = 0.25f;
const float Tank::COLLIDER_RADIUS = 2.0f;
const float Tank::COLLIDER_MASS = 10.0f;

Tank::Tank(ObjectID id, UserDBIndex ownerId)
	: GameObject(id, ownerId, true)
{
	ResetHP();
}

Tank::~Tank()
{
}

void Tank::Initiate(ObjectID id)
{
	ResetHP();
	_id = id;
	_isActivatable = true;
	_transform = { 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f };
	_hitTick = 0;
}

void Tank::Terminate()
{
	_id = INVALID_OBJECT_ID;
	_transform = { 0, };
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

void Tank::MoveForward(ULONGLONG tickDiff)
{
	//printf("pos: [%f, %f, %f]\n", _transform.Position.x, _transform.Position.y, _transform.Position.z);
	Vector3 forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
	_transform.Position.x = _transform.Position.x + forwardDirection.x * (tickDiff / 1000.f * 60.f) * POSITION_VELOCITY_WEIGHT;
	_transform.Position.y = _transform.Position.y + forwardDirection.y * (tickDiff / 1000.f * 60.f) * POSITION_VELOCITY_WEIGHT;
	_transform.Position.z = _transform.Position.z + forwardDirection.z * (tickDiff / 1000.f * 60.f) * POSITION_VELOCITY_WEIGHT;
	_dirty = true;

	OnUpdateTransform();
}

void Tank::MoveBackward(ULONGLONG tickDiff)
{
	Vector3 forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
	_transform.Position.x = _transform.Position.x - forwardDirection.x * (tickDiff / 1000.f * 60.f) * POSITION_VELOCITY_WEIGHT;
	_transform.Position.y = _transform.Position.y - forwardDirection.y * (tickDiff / 1000.f * 60.f) * POSITION_VELOCITY_WEIGHT;
	_transform.Position.z = _transform.Position.z - forwardDirection.z * (tickDiff / 1000.f * 60.f) * POSITION_VELOCITY_WEIGHT;
	_dirty = true;
	OnUpdateTransform();
}

void Tank::RotateRight(ULONGLONG tickDiff)
{
	const float angularSpeed = 3.14159265358979323846f / 1000.f * 60.f / 32.f;
	float radian = tickDiff * angularSpeed * ROTATION_VELOCITY_WEIGHT;

	_transform.Rotation = Quaternion::RotateZP(radian, _transform.Rotation);

	_dirty = true;
	OnUpdateTransform();
}

void Tank::RotateLeft(ULONGLONG tickDiff)
{
	const float angularVelocity = 3.14159265358979323846f / 1000.f * 60.f / 32.f;
	float radian = tickDiff * angularVelocity * ROTATION_VELOCITY_WEIGHT;

	_transform.Rotation = Quaternion::RotateZM(radian, _transform.Rotation);

	_dirty = true;
	OnUpdateTransform();
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
	if (IsAlive()) {
		if (_pCollider->IsCollided()) {
			OnHit(g_currentGameTick);
		}
		else {
			if (_isMovingFoward) {
				MoveForward(tickDiff);
			}
			if (_isMovingBackward) {
				MoveBackward(tickDiff);
			}
			if (_isRotatingLeft) {
				RotateLeft(tickDiff);
			}
			if (_isRotatingRight) {
				RotateRight(tickDiff);
			}
		}
		return;
	}

	if (g_currentGameTick - _hitTick > TICK_TANK_RESPAWN_INTERVAL) {
		OnRespawn();
		GamePacket::BroadcastRespawnTank(_id);
	}
	
	return;
}

Vector3 Tank::GetMovementVector()
{
	if (!(_isMovingFoward ^ _isMovingBackward)) {
		return { 0, 0, 0 };
	}
	
	Vector3 movement = GameObject::GetMovementVector();
	if (_isMovingBackward) {
		return movement * -1;
	}

	return movement;
}

BOOL Tank::IsDestroyed(ULONGLONG currentTick) const
{
	return !_isActivatable && (!_isAlive || _hitTick != 0);
}

void Tank::OnHit(ULONGLONG currentTick)
{
	UINT32 collisionFlag = _pCollider->GetCollisionKindnessFlag();
	if (collisionFlag & COLLIDER_KINDNESS_PROJECTILE) {
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
				
				// TODO: Find shooter and increase kill count.
			}
			GamePacket::BroadcastHit(_id, _ownerIndex, collisionFlag);
		}
	}

	if (collisionFlag & (COLLIDER_KINDNESS_TANK | COLLIDER_KINDNESS_SAME_POSITION)) {
	}
	Vector3 nextMovement = _pCollider->GetNextMovement();
	_transform.Position = nextMovement;
	_pCollider->UpdateCenterPosition(&_transform.Position);
}

void Tank::OnUpdateTransform()
{
	Vector3 movementVector = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation) * POSITION_VELOCITY_WEIGHT;
	_pCollider->Update(&_transform.Position, &movementVector);
}

void Tank::OnRespawn()
{
	GameObject::OnRespawn();
	_hitTick = 0;
	_pCollider->Activate();
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




