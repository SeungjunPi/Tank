#include "Tank.h"
#include "Global.h"
#include "GameEvent.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "PlayerManager.h"
#include "ObjectManager.h"

const float VELOCITY_WEIGHT = 0.75f;
const float Tank::COLLIDER_RADIUS = 1.0f;

Tank::Tank(ObjectID id, UserDBIndex ownerId)
	: GameObject(id, ownerId, true)
{
	_forwardDirection = { .0f, -1.0f, .0f };
}

Tank::~Tank()
{
}

void Tank::Initiate(ObjectID id)
{
	_id = id;
	_isActivatable = true;
	_forwardDirection = { .0f, -1.0f, .0f };
	_transform = { 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f };
	_hitTick = 0;
}

void Tank::Terminate()
{
	_id = INVALID_OBJECT_ID;
	_forwardDirection = { 0, };
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
	_forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
	_transform.Position.x = _transform.Position.x + _forwardDirection.x * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_transform.Position.y = _transform.Position.y + _forwardDirection.y * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_transform.Position.z = _transform.Position.z + _forwardDirection.z * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_dirty = true;

	OnUpdateTransform();
}

void Tank::MoveBackward(ULONGLONG tickDiff)
{
	_forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
	_transform.Position.x = _transform.Position.x - _forwardDirection.x * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_transform.Position.y = _transform.Position.y - _forwardDirection.y * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_transform.Position.z = _transform.Position.z - _forwardDirection.z * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_dirty = true;
	OnUpdateTransform();
}

void Tank::RotateRight(ULONGLONG tickDiff)
{
	const float angularVelocity = 3.14159265358979323846f / 1000.f * 60.f / 32.f;
	float radian = tickDiff * angularVelocity * VELOCITY_WEIGHT;

	_transform.Rotation = Quaternion::RotateZP(radian, _transform.Rotation);

	_dirty = true;
	OnUpdateTransform();
}

void Tank::RotateLeft(ULONGLONG tickDiff)
{
	const float angularVelocity = 3.14159265358979323846f / 1000.f * 60.f / 32.f;
	float radian = tickDiff * angularVelocity * VELOCITY_WEIGHT;

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

void Tank::OnFrame(ULONGLONG tickDiff)
{
	if (IsAlive()) {
		memcpy(&_prevTransform, &_transform, sizeof(Transform));

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

		return;
	}

	if (g_currentGameTick - _hitTick > TICK_TANK_RESPAWN_INTERVAL) {
		OnRespawn();
		GamePacket::BroadcastRespawnTank(_id);
	}
	
	return;
}

BOOL Tank::IsDestroyed(ULONGLONG currentTick) const
{
	return !_isActivatable && (!_isAlive || _hitTick != 0);
}

void Tank::OnHit(ULONGLONG currentTick)
{
	printf("[JUNLOG]onhit tank\n");

	ColliderID colliderIDs[MAX_SIMULTANEOUS_COLLISIONS];
	UINT16 countColliders = _pCollider->GetCollidingIDs(colliderIDs);
	for (UINT16 i = 0; i < countColliders; ++i) {
		Collider* pOtherCollider = g_pCollisionManager->GetAttachedColliderPtr(colliderIDs[i]);
		ObjectID otherObjID = pOtherCollider->GetAttachedObjectPtr()->GetID();
		GameObject* pOtherObj = g_objectManager.GetObjectPtrOrNull(otherObjID);
		switch (otherObjID.type) {
		case GAME_OBJECT_TYPE_PROJECTILE:
			// hit
			_pCollider->Deactivate();
			_hitTick = currentTick;
			_isAlive = false;
			_isMovingFoward = false;
			_isMovingBackward = false;
			_isRotatingLeft = false;
			_isRotatingRight = false;
			g_playerManager.IncreaseDeathCount(_ownerIndex);
			GamePacket::BroadcastTankHit(_id, otherObjID, pOtherObj->GetOwnerId(), _ownerIndex);
			break;
		case GAME_OBJECT_TYPE_TANK:
			memcpy(&_transform, &_prevTransform, sizeof(Transform));
			_pCollider->UpdateCenterPosition(&_transform.Position);
			
			break;
		case GAME_OBJECT_TYPE_OBSTACLE:
			break;
		}
	}
}

void Tank::OnUpdateTransform()
{
	_pCollider->UpdateCenterPosition(&_transform.Position);
}

void Tank::OnRespawn()
{
	GameObject::OnRespawn();
	_hitTick = 0;
	_pCollider->Activate();
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




