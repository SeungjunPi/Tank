#include "Tank.h"
#include "StaticData.h"
#include "Global.h"
#include "GameEvent.h"
#include "Collider.h"
#include "ICollisionManager.h"
#include "AllocObjectManager.h"
#include "Camera.h"
#include "Player.h"

const float POSITION_VELOCITY_WEIGHT = 0.5f;
const float ROTATION_VELOCITY_WEIGHT = 0.25f;
const float Tank::COLLIDER_RADIUS = 2.0f;
const ULONGLONG Tank::MACHINE_GUN_DELAY = 250;
const float Tank::COLLIDER_MASS = 10.0f;

Tank::Tank(ObjectID id, UserDBIndex ownerID)
	: GameObject(id, ownerID, true)
{	
	_model = g_pTankModel;

	ResetHP();
}

Tank::~Tank()
{
}

void Tank::Initiate(ObjectID id)
{
	_id = id;
	_isActivatable = true;
	_transform = { 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f };
	_model = g_pTankModel;
	_hitTick = 0;
	ResetHP();
}

void Tank::Terminate()
{
	_id = INVALID_OBJECT_ID;
	_transform = { 0, };
	_model = { nullptr, 0 };
}

void Tank::SetMovementState(bool forward, bool backward, bool left, bool right)
{
	_isMovingFoward = forward;
	_isMovingBackward = backward;
	_isRotatingLeft = left;
	_isRotatingRight = right;
}


void Tank::StartMove(EMovement movement)
{
	switch (movement) {
	case EMovement::FORWARD:
		_isMovingFoward = true;
		break;
	case EMovement::BACKWARD:
		_isMovingBackward = true;
		break;
	default:
		__debugbreak();
	}

	
}

void Tank::EndMove(EMovement movement)
{
	switch (movement) {
	case EMovement::FORWARD:
		_isMovingFoward = false;
		break;
	case EMovement::BACKWARD:
		_isMovingBackward = false;
		break;
	default:
		__debugbreak();
	}
}

void Tank::EndMove(EMovement movement, const Transform* pTransform)
{
	switch (movement) {
	case EMovement::FORWARD:
		_isMovingFoward = false;
		memcpy(&_transform, pTransform, sizeof(Transform));
		break;
	case EMovement::BACKWARD:
		_isMovingBackward = false;
		memcpy(&_transform, pTransform, sizeof(Transform));
		break;
	default:
		__debugbreak();
	}
	OnUpdateTransform();
}

void Tank::StartRotate(ERotation rotation)
{
	switch (rotation) {
	case ERotation::LEFT:
		_isRotatingLeft = true;
		break;
	case ERotation::RIGHT:
		_isRotatingRight = true;
		break;
	default:
		__debugbreak();
	}
}

void Tank::EndRotate(ERotation rotation)
{
	switch (rotation) {
	case ERotation::LEFT:
		_isRotatingLeft = false;
		break;
	case ERotation::RIGHT:
		_isRotatingRight = false;
		break;
	default:
		__debugbreak();
	}
}

void Tank::EndRotate(ERotation rotation, const Transform* pTransform)
{
	switch (rotation) {
	case ERotation::LEFT:
		memcpy(&_transform, pTransform, sizeof(Transform));
		_isRotatingLeft = false;
		break;
	case ERotation::RIGHT:
		memcpy(&_transform, pTransform, sizeof(Transform));
		_isRotatingRight = false;
		break;
	default:
		__debugbreak();
	}
	OnUpdateTransform();
}

void Tank::MoveForward(ULONGLONG tickDiff)
{
	
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
	const float angularVelocity = 3.14159265358979323846f / 1000.f * 60.f / 32.f;
	float radian = tickDiff * angularVelocity * ROTATION_VELOCITY_WEIGHT;

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

	Vector3 v = _model.vertices[0].v;
	v = Vector3::Rotate(v, rotation);
	v.x += position.x;
	v.y += position.y;
	v.z += position.z;
	*out_position = v;

	const Vector3 forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);

	memcpy(out_direction, &forwardDirection, sizeof(Vector3));
}

void Tank::GetTurretInfo(Transform* out_transform) const
{
	Quaternion rotation = _transform.Rotation;

	Vector3 v = _model.vertices[0].v;
	v = Vector3::Rotate(v, rotation);
	v.x += _transform.Position.x;
	v.y += _transform.Position.y;
	v.z += _transform.Position.z;

	out_transform->Position = v;
	out_transform->Rotation = _transform.Rotation;
}

bool Tank::CanFireMachineGun() const
{
	return _lastMachineGunFiringTick + MACHINE_GUN_DELAY <= g_currentGameTick;
}

void Tank::OnFiringMachineGun(ULONGLONG currentTick)
{
	_lastMachineGunFiringTick = currentTick;
	// Send Fire
}

void Tank::ResetHP()
{
	_hp = 5;
}

BOOL Tank::IsDestroyed(ULONGLONG currentTick) const
{
	return !_isActivatable && (!_isAlive || _hitTick != 0);
}

void Tank::OnFrame(ULONGLONG tickDiff)
{
	if (!IsAlive()) {
		return;
	}

	if (_ownerID == g_pPlayer->GetUserID()) {
		// check 
	}
	
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

	if (_flagFiringMachineGun) {
		OnFiringMachineGun(g_currentGameTick); // 충돌과 무관하게 발사
	}
	return;
}

void Tank::OnHit(ULONGLONG currentTick)
{
	UINT32 collisionFlag = _pCollider->GetCollisionKindnessFlag();
	if (collisionFlag & COLLIDER_KINDNESS_PROJECTILE) {
		// run animation? or gen particle?
	}

	if (collisionFlag & (COLLIDER_KINDNESS_TANK || COLLIDER_KINDNESS_SAME_POSITION)) {
		
	}

	Vector3 nextMovement = _pCollider->GetNextMovement();
	_transform.Position = nextMovement;
	OnUpdateTransform();
}

void Tank::OnUpdateTransform()
{
	//printf("pos: [%f, %f, %f]\n", _transform.Position.x, _transform.Position.y, _transform.Position.z);
	Vector3 movementVector = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation) * POSITION_VELOCITY_WEIGHT;
	_pCollider->Update(&_transform.Position, &movementVector);
	if (g_pPlayer->GetUserID() == _ownerID) {
		g_pCamera->UpdateTransf(&_transform);
	}
}

void Tank::OnRespawn()
{
	GameObject::OnRespawn();
	_hitTick = 0;
	_pCollider->Activate();
	ResetHP();
}

void Tank::OnHitByProjectile(ULONGLONG currentTick)
{
	if (_hp > 0) {
		if (--_hp == 0) {
			_hitTick = currentTick;
			_isAlive = false;
			_isMovingFoward = false;
			_isMovingBackward = false;
			_isRotatingLeft = false;
			_isRotatingRight = false;
			_pCollider->Deactivate();
			if (g_pPlayer->GetUserID() == _ownerID) {
				g_pPlayer->IncreaseDeath();
			}
		}
	}
}

