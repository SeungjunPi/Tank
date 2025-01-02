#include "Projectile.h"
#include "Global.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "PlayerManager.h"

const float Projectile::COLLIDER_RADIUS = 1.f;

void Projectile::Initiate(ObjectID id, Transform* transform, UserDBIndex ownerId, Collider* pCollider)
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
	_pCollider = pCollider;

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

void Projectile::OnHit(ULONGLONG currentTick)
{
	if (_hitTick != 0) {
		__debugbreak();
	}
	printf("[JUNLOG]OnHit projectile\n");

	ColliderID colliderIDs[MAX_SIMULTANEOUS_COLLISIONS];
	UINT16 countColliders = _pCollider->GetCollidingIDs(colliderIDs);
	for (UINT16 i = 0; i < countColliders; ++i) {
		Collider* pOtherCollider = g_pCollisionManager->GetAttachedColliderPtr(colliderIDs[i]);
		ObjectID otherObjID = pOtherCollider->GetObjectID();
		EGameObjectKind objKind = g_objectManager.FindObjectKindByID(otherObjID);
		switch (objKind) {
		case GAME_OBJECT_KIND_PROJECTILE:
			// hit
			g_playerManager.IncreaseKillCount(_ownerIndex);
			_hitTick = currentTick;
			_pCollider->Deactivate();
			printf("Projectile [%u(%u)] hit Tank [%u] \n", _id, _ownerIndex, otherObjID);
			break;
		case GAME_OBJECT_KIND_TANK:
			// Fall Through
		case GAME_OBJECT_KIND_OBSTACLE:
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
