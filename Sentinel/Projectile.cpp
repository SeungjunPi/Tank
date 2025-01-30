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

	_movementSpeed = PROJECTILE_MOVEMENT_SPEED;
	_rotationSpeed = 0.0f;
	_mass = PROJECTILE_COLLIDER_MASS;
	_radius = PROJECTILE_COLLIDER_RADIUS;
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
}

BOOL Projectile::IsDestroyed(ULONGLONG currentTick) const
{
	return !_isActivatable && (!_isAlive || _hitTick != 0);
}

void Projectile::OnHitWith(ULONGLONG currentTick, GameObject* other)
{
	if (_hitTick != 0) {
		__debugbreak();
	}

	ObjectID otherID = other->GetID();
	if (otherID.type == GAME_OBJECT_TYPE_TANK) {
		g_playerManager.IncreaseHitCount(_ownerIndex);
		_hitTick = currentTick;
		_pCollider->Deactivate();
		printf("Projectile [%u(%u)] hit Tank\n", _id.key, _ownerIndex);
		return;
	}
}

BOOL Projectile::IsTimeout() const
{
	const static ULONGLONG lifeTime = 5000;

	if (g_previousGameTick - _genTick > lifeTime) {
		return true;
	}

	return false;
}
