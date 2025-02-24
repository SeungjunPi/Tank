#include "Collider.h"

Collider::Collider()
{
}

void Collider::ResetCollisionFlag()
{
	_collisionKindnessFlag = 0;
}

void Collider::UpdatePhysicsComponentFromGameObject()
{
	memcpy(&_transform, _pObjectTransform, sizeof(Transform));
	memcpy(&_velocity, _pObjectVelocity, sizeof(Transform));
}

void Collider::OverwriteComputedResultsToGameObject()
{
	memcpy(&_nextTransform, _pObjectTransform, sizeof(Transform));
	memcpy(&_nextVelocity, _pObjectVelocity, sizeof(Transform));
}

void Collider::GetComputedResults(Transform* out_transform, Transform* out_velocity) const
{
	memcpy(out_transform, &_nextTransform, sizeof(Transform));
	memcpy(out_velocity, &_nextVelocity, sizeof(Transform));
}

void Collider::Initiate(float radius, GameObject* pObj, const Vector3* center, UINT32 kindness)
{
	_radius = radius;
	_pObject = pObj;
	_kindness = kindness;
	Activate();
}

void Collider::Clear()
{
	_id = INVALID_COLLIDER_ID;
	_isActive = FALSE;
	_collisionKindnessFlag = 0;
	_pObject = nullptr;
	_kindness = 0;

	Deactivate();
}




