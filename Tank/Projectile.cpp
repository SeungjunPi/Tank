#include "TankPch.h"

#include "Projectile.h"
#include "Global.h"
#include "StaticData.h"
#include "Collider.h"
#include "AllocObjectManager.h"
#include "ICollisionManager.h"
#include "Player.h"

void Projectile::Initiate(ObjectID id, Transform* transform, UserDBIndex ownerID)
{
	_id = id;
	_ownerID = ownerID;
	memcpy(&_transform, transform, sizeof(Transform));

	_model = g_pProjectileModel;
	_genTick = g_previousGameTick;
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

void Projectile::PreProcessMovementState()
{
	_translationDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
	_translationSpeed = PROJECTILE_TRANSLATION_SPEED;
	_rotationAngle = 0.f;
}

void Projectile::OnHitWith(ULONGLONG currentTick, GameObject* other)
{
	
}

void Projectile::OnHitServer(ULONGLONG currentTick, GameObject* other)
{
	if (other->GetID().type == GAME_OBJECT_TYPE_TANK) {
		if (_hitTick != 0) {
			return;
		}

		_hitTick = currentTick;
		_pCollider->Deactivate();
	}
}

void Projectile::OnUpdateTransform()
{
	
}
