#include "Tank.h"
#include "StaticData.h"
#include "Global.h"
#include "GameEvent.h"
#include "Collider.h"
#include "ICollisionManager.h"
#include "AllocObjectManager.h"
#include "Camera.h"

const float POSITION_VELOCITY_WEIGHT = 0.5f;
const float ROTATION_VELOCITY_WEIGHT = 0.25f;
const float Tank::COLLIDER_RADIUS = 1.0f;

Tank::Tank(ObjectID id, UserDBIndex ownerID)
	: GameObject(id, ownerID, true)
{
	_forwardDirection = { .0f, -1.0f, .0f };
	
	_model = g_pTankModel;
	_colliderSize = 1;

	ResetHP();
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
	_model = g_pTankModel;
	_colliderSize = 1;
	_hitTick = 0;
	ResetHP();
}

void Tank::Terminate()
{
	_id = INVALID_OBJECT_ID;
	_forwardDirection = { 0, };
	_transform = { 0, };
	_model = { nullptr, 0 };
	_colliderSize = 0;
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

void Tank::EndMove(EMOVEMENT movement, const Transform* pTransform)
{
	switch (movement) {
	case EMOVEMENT::FORWARD:
		_isMovingFoward = false;
		memcpy(&_transform, pTransform, sizeof(Transform));
		break;
	case EMOVEMENT::BACKWARD:
		_isMovingBackward = false;
		memcpy(&_transform, pTransform, sizeof(Transform));
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

void Tank::EndRotate(EROTATION rotation, const Transform* pTransform)
{
	switch (rotation) {
	case EROTATION::LEFT:
		memcpy(&_transform, pTransform, sizeof(Transform));
		_isRotatingLeft = false;
		break;
	case EROTATION::RIGHT:
		memcpy(&_transform, pTransform, sizeof(Transform));
		_isRotatingRight = false;
		break;
	default:
		__debugbreak();
	}
}

void Tank::MoveForward(ULONGLONG tickDiff)
{
	_forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
	_transform.Position.x = _transform.Position.x + _forwardDirection.x * (tickDiff / 1000.f * 60.f) * POSITION_VELOCITY_WEIGHT;
	_transform.Position.y = _transform.Position.y + _forwardDirection.y * (tickDiff / 1000.f * 60.f) * POSITION_VELOCITY_WEIGHT;
	_transform.Position.z = _transform.Position.z + _forwardDirection.z * (tickDiff / 1000.f * 60.f) * POSITION_VELOCITY_WEIGHT;
	_dirty = true;
	OnUpdateTransform();
}

void Tank::MoveBackward(ULONGLONG tickDiff)
{
	_forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
	_transform.Position.x = _transform.Position.x - _forwardDirection.x * (tickDiff / 1000.f * 60.f) * POSITION_VELOCITY_WEIGHT;
	_transform.Position.y = _transform.Position.y - _forwardDirection.y * (tickDiff / 1000.f * 60.f) * POSITION_VELOCITY_WEIGHT;
	_transform.Position.z = _transform.Position.z - _forwardDirection.z * (tickDiff / 1000.f * 60.f) * POSITION_VELOCITY_WEIGHT;
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

	return;
}

void Tank::OnHit(ULONGLONG currentTick)
{
	ColliderID colliderIDs[MAX_SIMULTANEOUS_COLLISIONS];
	UINT16 countColliders = _pCollider->GetCollidingIDs(colliderIDs);
	for (UINT16 i = 0; i < countColliders; ++i) {
		Collider* pOtherCollider = g_pCollisionManager->GetAttachedColliderPtr(colliderIDs[i]);
		ObjectID otherObjID = pOtherCollider->GetAttachedObjectPtr()->GetID();
		GameObject* pOtherObj = g_objectManager.GetObjectPtrOrNull(otherObjID);
		switch (otherObjID.type) {
		case GAME_OBJECT_TYPE_PROJECTILE:
			// Do Nothing yet..
			break;
		case GAME_OBJECT_TYPE_TANK:
			memcpy(&_transform, &_prevTransform, sizeof(Transform));
			_pCollider->UpdateCenterPosition(&_transform.Position);
			break;
		case GAME_OBJECT_TYPE_OBSTACLE:
			// Do Nothing yet..
			break;
		}
	}

}

void Tank::OnUpdateTransform()
{
	_pCollider->UpdateCenterPosition(&_transform.Position);
	if (g_playerId == _ownerID) {
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
			if (g_playerId == _ownerID) {
				g_score.death++;
			}
		}
	}
}

