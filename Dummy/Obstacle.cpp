
#include "Obstacle.h"
#include "StaticData.h"

void Obstacle::Initiate(ObjectID id, Transform* pInitTransform)
{
	/*memcpy(&_transform, pInitTransform, sizeof(Transform));
	_id = id;
	_model = g_pObstacleModel;
	_hitTick = 0;*/
}

void Obstacle::Terminate()
{
	_id = INVALID_OBJECT_ID;
}

void Obstacle::OnHitWith(ULONGLONG currentTick, GameObject* pOther)
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
