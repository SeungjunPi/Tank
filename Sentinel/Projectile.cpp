#include "Projectile.h"
#include "Global.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "PlayerManager.h"

const float Projectile::COLLIDER_RADIUS = 1.f;

void Projectile::Initiate(ObjectID id, Transform* transform, UserDBIndex ownerId)
{
	_id = id;
	memcpy(&_transform, transform, sizeof(Transform));

	_forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
	_transform.Position.x += _forwardDirection.x * 1.2f;
	_transform.Position.y += _forwardDirection.y * 1.2f;
	_transform.Position.z += _forwardDirection.z * 1.2f;

	_dirty = true;
	_genTick = g_previousGameTick;
	
	_ownerIndex = ownerId;

	OnUpdateTransform();
}

void Projectile::Terminate()
{

}

void Projectile::OnFrame(ULONGLONG tickDiff)
{
	if (IsTimeout() || _hitTick != 0) {
		_isAlive = false;
		return;
	}

	Move(tickDiff);
}

BOOL Projectile::IsDestroyed(ULONGLONG currentTick) const
{
	return !_isActivatable && (!_isAlive || _hitTick != 0);
}

void Projectile::OnHit(ULONGLONG currentTick)
{
	if (_hitTick != 0) {
		__debugbreak();
	}

	ColliderID colliderIDs[MAX_SIMULTANEOUS_COLLISIONS];
	UINT16 countColliders = _pCollider->GetCollidingIDs(colliderIDs);
	for (UINT16 i = 0; i < countColliders; ++i) {
		Collider* pOtherCollider = g_pCollisionManager->GetAttachedColliderPtr(colliderIDs[i]);
		ObjectID otherObjID = pOtherCollider->GetAttachedObjectPtr()->GetID();
		switch (otherObjID.type) {
		case GAME_OBJECT_TYPE_PROJECTILE:
			// hit
			printf("[JUNLOG]Projectile Hit Other Projectile\n");
			break;
		case GAME_OBJECT_TYPE_TANK:
			g_playerManager.IncreaseKillCount(_ownerIndex);
			_hitTick = currentTick;
			_pCollider->Deactivate();
			printf("Projectile [%u(%u)] hit Tank [%u] \n", _id.key, _ownerIndex, otherObjID.key);
			break;
			// Fall Through
		case GAME_OBJECT_TYPE_OBSTACLE:
			break;
		}
	}		
}

void Projectile::OnUpdateTransform()
{
	_pCollider->UpdateCenterPosition(&_transform.Position);
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
