#include "Projectile.h"
#include "Global.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "PlayerManager.h"
#include "StaticData.h"

const float Projectile::COLLIDER_RADIUS = 1.f;
const float Projectile::COLLIDER_MASS = 0.1f;
const float Projectile::SPEED_PER_MS = 40.0f / 1000.0f;

void Projectile::Initiate(ObjectID id, Transform* transform, UserDBIndex ownerId)
{
	assert(_pCollider != nullptr);
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

	if (_pCollider->IsCollided()) {
		OnHit(g_currentGameTick);
	}
	else {
		Move(tickDiff);
	}
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

	UINT32 collisionFlag = _pCollider->GetCollisionKindnessFlag();
	if (collisionFlag & COLLIDER_KINDNESS_TANK) {
		g_playerManager.IncreaseHitCount(_ownerIndex);
		_hitTick = currentTick;
		_pCollider->Deactivate();
		printf("Projectile [%u(%u)] hit Tank\n", _id.key, _ownerIndex);
		return;
	}

	if (collisionFlag & COLLIDER_KINDNESS_PROJECTILE) {
		// hit other projectile, ignore hit
		Move(g_previousGameTick - g_currentGameTick);
	}
	
}

void Projectile::OnUpdateTransform()
{
	Vector3 movementVector = _forwardDirection * SPEED_PER_MS;
	_pCollider->Update(&_transform.Position, &movementVector);
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
	_transform.Position.x += _forwardDirection.x * SPEED_PER_MS * tickDiff;
	_transform.Position.y += _forwardDirection.y * SPEED_PER_MS * tickDiff;
	_transform.Position.z += _forwardDirection.z * SPEED_PER_MS * tickDiff;
	_dirty = true;
	OnUpdateTransform();
}
