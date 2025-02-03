#include "Collider.h"

Collider::Collider()
{
}

void Collider::UpdateCenter(const Vector3& position)
{
	_center = position;
}

void Collider::ResetCollisionFlag()
{
	_collisionKindnessFlag = 0;
}

void Collider::Initiate(float radius, GameObject* pObj, const Vector3* center, UINT32 kindness)
{
	_radius = radius;
	_pObject = pObj;
	_kindness = kindness;
	UpdateCenter(*center);
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




