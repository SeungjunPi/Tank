#include "Collider.h"

Collider::Collider()
{
}

void Collider::Update(Vector3& centerPosition, Vector3& movementDirection, float movementSpeed)
{
	_center = centerPosition;
	_movementDirection = movementDirection;
	_movementSpeed = movementSpeed;
}

void Collider::Update(const Vector3* center, const Vector3* movementDirection, float movementSpeed)
{
	memcpy(&_center, center, sizeof(Vector3));
	memcpy(&_movementDirection, movementDirection, sizeof(Vector3));
	_movementSpeed = movementSpeed;
}

void Collider::ResetCollisionFlag()
{
	_collisionKindnessFlag = 0;
}

void Collider::Initiate(float radius, GameObject* pObj, const Vector3* center, const Vector3* direction, float mass, UINT32 kindness)
{
	Update(center, direction, 0.0f);
	_radius = radius;
	_mass = mass;
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




