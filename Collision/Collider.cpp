#include "Collider.h"

Collider::Collider()
{
}

void Collider::UpdateCenterPosition(const Vector3* position)
{
	memcpy(&_center, position, sizeof(Vector3));
}

void Collider::Update(const Vector3* center, const Vector3* velocity)
{
	memcpy(&_center, center, sizeof(Vector3));
	memcpy(&_velocity, velocity, sizeof(Vector3));
}

void Collider::ResetCollisionFlag()
{
	_collisionKindnessFlag = 0;
}

void Collider::Initiate(float radius, GameObject* pObj, const Vector3* center, float mass, UINT32 kindness)
{
	UpdateCenterPosition(center);
	_velocity = { 0, };
	_radius = radius;
	_mass = mass;
	_pObject = pObj;
	_kindness = kindness;
	Activate();
}

void Collider::Clear()
{
	_id = INVALID_COLLIDER_ID;
	memset(&_center, 0, sizeof(Vector3));
	_radius = 0.f;
	memset(&_velocity, 0, sizeof(Vector3));
	memset(&_nextMovement, 0, sizeof(Vector3));
	_mass = 0.f;

	_isActive = FALSE;
	_collisionKindnessFlag = 0;
	_pObject = nullptr;
	_kindness = 0;

	Deactivate();
}




