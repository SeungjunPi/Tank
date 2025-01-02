#include "Projectile.h"
#include "Global.h"
#include "Collider.h"

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
	printf("[JUNLOG]OnHit projectile\n");
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
