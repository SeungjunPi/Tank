
#include "Obstacle.h"
#include "StaticData.h"

void Obstacle::Initiate(UINT id, Transform* pInitTransform)
{
	memcpy(&_transform, pInitTransform, sizeof(Transform));
	_id = id;
	_model = g_pObstacleModel;
	_colliderSize = 1;
	_hitTick = 0;
}

void Obstacle::Terminate()
{
	_id = 0;
}

void Obstacle::OnHit(ULONGLONG currentTick)
{
	if (_hitTick == 0) {
		_hitTick = currentTick;
	}
}

BOOL Obstacle::IsDestroyed(ULONGLONG currentTick) const
{
	if (_hitTick == 0) {
		return false;
	}

	return (currentTick - _hitTick) > DESTROY_DELAY;
}
