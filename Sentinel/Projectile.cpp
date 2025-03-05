#include "Projectile.h"
#include "Global.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "PlayerManager.h"
#include "StaticData.h"
#include "ServerPacketHandler.h"

void Projectile::Initiate(ObjectID id, const Transform* transform, UserDBIndex ownerId)
{
	assert(_pCollider != nullptr);
	_id = id;
	_ownerIndex = ownerId;
	memcpy(&_physicalComponent.transform, transform, sizeof(Transform));
	
	_genTick = g_previousGameTick;

	_physicalComponent.velocity = Vector3::Rotate(FORWARD_DIRECTION, _physicalComponent.transform.Rotation) * PROJECTILE_TRANSLATION_SPEED;
	_physicalComponent.mass = PROJECTILE_COLLIDER_MASS;
	_physicalComponent.radius = PROJECTILE_COLLIDER_RADIUS;

	_pCollider->UpdatePhysicsComponentFromGameObject();
}

void Projectile::Terminate()
{

}

void Projectile::Tick(ULONGLONG tickDiff)
{
	GameObject::Tick(tickDiff);
	if (IsTimeout() || _hitTick != 0) {
		_isAlive = false;
		_pCollider->Deactivate();
		return;
	}
}

BOOL Projectile::IsDestroyed(ULONGLONG currentTick) const
{
	return !_isActivatable && (!_isAlive || _hitTick != 0);
}

void Projectile::OnHitWith(ULONGLONG currentTick, GameObject* other)
{
	// printf("Projectile OnHit %llu\n", _hitTick);
	if (_hitTick != 0) {
		return;
	}

	if (_ownerIndex == other->GetOwnerId()) {
		return;
	}

	ObjectID otherID = other->GetID();
	if (otherID.type == GAME_OBJECT_TYPE_TANK) {
		_hitTick = currentTick;
		_pCollider->Deactivate();
		printf("Projectile [%u(%u)] hit Tank\n", _id.key, _ownerIndex);
		return;
	}
}

void Projectile::OnUpdateTransform()
{
}

BOOL Projectile::IsTimeout() const
{
	const static ULONGLONG lifeTime = 5000;

	if (g_previousGameTick - _genTick > lifeTime) {
		return true;
	}

	return false;
}
