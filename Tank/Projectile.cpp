#include "Projectile.h"
#include "Global.h"
#include "StaticData.h"

void Projectile::Initiate(UINT32 id, Transform* transform)
{
	_id = id;
	memcpy(&_transform, transform, sizeof(Transform));

	_forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
	_transform.Position.x += _forwardDirection.x;
	_transform.Position.y += _forwardDirection.y;
	_transform.Position.z += _forwardDirection.z;

	_model = g_pProjectileModel;
	_colliderSize = 1.0f;
	_dirty = true;
	_genTick = g_previousGameTick;
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
	const static float SPEED_PER_MS = 4.0f / 1000.0f;
	_transform.Position.x += _forwardDirection.x * SPEED_PER_MS * tickDiff;
	_transform.Position.y += _forwardDirection.y * SPEED_PER_MS * tickDiff;
	_transform.Position.z += _forwardDirection.z * SPEED_PER_MS * tickDiff;
	_dirty = true;
}
