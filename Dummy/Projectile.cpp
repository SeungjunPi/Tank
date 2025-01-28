#include "TankPch.h"

#include "Projectile.h"
#include "Global.h"
#include "StaticData.h"
#include "Collider.h"
#include "AllocObjectManager.h"
#include "ICollisionManager.h"

const float POSITION_VELOCITY_WEIGHT = 1.5f;
const float Projectile::COLLIDER_RADIUS = 1.0f;

void Projectile::Initiate(ObjectID id, Transform* transform, UserDBIndex ownerID)
{
	_id = id;
	_ownerID = ownerID;
	memcpy(&_transform, transform, sizeof(Transform));

	_forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
	_transform.Position.x += _forwardDirection.x * 1.2f;
	_transform.Position.y += _forwardDirection.y * 1.2f;
	_transform.Position.z += _forwardDirection.z * 1.2f;

	_model = g_pProjectileModel;
	_colliderSize = 1.0f;
	_dirty = true;
	_genTick = g_previousGameTick;

	OnUpdateTransform();
}

void Projectile::Terminate()
{

}

void Projectile::OnFrame(ULONGLONG tickDiff)
{
	if (IsTimeout()) {
		_isAlive = false;
		return;
	}

	Move(tickDiff);
}

BOOL Projectile::IsDestroyed(ULONGLONG currentTick) const
{
	return !_isActivatable && (!_isAlive || _hitTick != 0);
}

BOOL Projectile::IsTimeout() const
{
	const static ULONGLONG lifeTime = 5000;
	
	if (g_previousGameTick - _genTick > lifeTime) {
		return true;
	}

	return false;
}

void Projectile::Move(ULONGLONG tickDiff)
{
	const static float SPEED_PER_MS = 40.0f / 1000.0f;
	_transform.Position.x += _forwardDirection.x * SPEED_PER_MS * tickDiff;
	_transform.Position.y += _forwardDirection.y * SPEED_PER_MS * tickDiff;
	_transform.Position.z += _forwardDirection.z * SPEED_PER_MS * tickDiff;
	_dirty = true;
	OnUpdateTransform();
}

void Projectile::OnHit(ULONGLONG currentTick)
{
	// Do Nothing yet..
	ColliderID colliderIDs[MAX_SIMULTANEOUS_COLLISIONS];
	UINT16 countColliders = _pCollider->GetCollidingIDs(colliderIDs);
	for (UINT16 i = 0; i < countColliders; ++i) {
		Collider* pOtherCollider = g_pCollisionManager->GetAttachedColliderPtr(colliderIDs[i]);
		ObjectID otherObjID = pOtherCollider->GetAttachedObjectPtr()->GetID();
		GameObject* pOtherObj = g_objectManager.GetObjectPtrOrNull(otherObjID);
		switch (otherObjID.type) {
		case GAME_OBJECT_TYPE_PROJECTILE:
			// TODO: do something..?
			break;
		case GAME_OBJECT_TYPE_TANK:
			// TODO: do something..?
			break;
		case GAME_OBJECT_TYPE_OBSTACLE:
			break;
		}
	}
}

void Projectile::OnUpdateTransform()
{
	_pCollider->UpdateCenterPosition(&_transform.Position);
}

void Projectile::OnHitTank(ULONGLONG currentTick)
{
	if (_hitTick != 0) {
		__debugbreak();
	}

	_hitTick = currentTick;
	_pCollider->Deactivate();
}
