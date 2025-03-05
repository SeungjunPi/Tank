#include "TankPch.h"

#include "Projectile.h"
#include "Global.h"
#include "StaticData.h"
#include "Collider.h"
#include "AllocObjectManager.h"
#include "IStableFlow.h"
#include "Player.h"

void Projectile::Initiate(ObjectID id, const Transform* transform, UserDBIndex ownerID)
{
	_id = id;
	_ownerID = ownerID;
	memcpy(&_physicalComponent.transform, transform, sizeof(Transform));

	_model = g_pProjectileModel;
	_genTick = g_previousGameTick;
	_physicalComponent.velocity = Vector3::Rotate(FORWARD_DIRECTION, _physicalComponent.transform.Rotation) * PROJECTILE_TRANSLATION_SPEED;
	_physicalComponent.mass = PROJECTILE_COLLIDER_MASS;
	_physicalComponent.radius = PROJECTILE_COLLIDER_RADIUS;
}

void Projectile::Terminate()
{

}

void Projectile::Tick(ULONGLONG tickDiff)
{
	GameObject::Tick(tickDiff);
	if (IsTimeout()) {
		_isAlive = false;
		_pCollider->Deactivate();
		return;
	}
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

void Projectile::OnHitWith(ULONGLONG currentTick, GameObject* other)
{
	
}

void Projectile::OnHitServer(ULONGLONG currentTick, GameObject* other)
{
	_hitTick = currentTick;
	_pCollider->Deactivate();
}

void Projectile::OnUpdateTransform()
{
	
}
