#include "Projectile.h"
#include "Global.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "PlayerManager.h"
#include "StaticData.h"
#include "GameEvent.h"
#include "CommonData.h"


void Projectile::Initiate(ObjectID id, Transform* transform, UserDBIndex ownerId)
{
	assert(_pCollider != nullptr);
	_id = id;
	_ownerIndex = ownerId;
	memcpy(&_transform, transform, sizeof(Transform));
	
	_genTick = g_previousGameTick;

	_translationSpeed = PROJECTILE_TRANSLATION_SPEED;
	_mass = PROJECTILE_COLLIDER_MASS;
	_radius = PROJECTILE_COLLIDER_RADIUS;
	SyncTransformWithCollider();
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

void Projectile::PreProcessMovementState()
{
	_translationDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
	_translationSpeed = PROJECTILE_TRANSLATION_SPEED;
	_rotationAngle = 0.f;
}

void Projectile::OnHitWith(ULONGLONG currentTick, GameObject* other)
{
	// printf("Projectile OnHit %llu\n", _hitTick);
	if (_hitTick != 0) {
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
