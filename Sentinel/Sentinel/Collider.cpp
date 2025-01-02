#include "Collider.h"

Collider::Collider()
	: _id(INVALID_COLLIDER_ID)
	, _radius(0)
	, _countColliding(0)
	, _objectID(INVALID_OBJECT_ID)
{
	for (size_t i = 0; i < MAX_SIMULTANEOUS_COLLISIONS; ++i) {
		_collidingIDs[i] = INVALID_COLLIDER_ID;
	}
}

void Collider::UpdateCenterPosition(const Vector3* position)
{
	memcpy(&_center, position, sizeof(Vector3));
}

UINT16 Collider::GetCollidingIDs(ColliderID* out)
{
	if (_countColliding > 0) {
		memcpy(out, _collidingIDs, sizeof(ColliderID) * _countColliding);
	}
	return _countColliding;
}

void Collider::ClearCollisionInfo()
{
	_countColliding = 0;
}


void Collider::AddCollidingID(ColliderID otherColliderID)
{
	if (_countColliding == MAX_SIMULTANEOUS_COLLISIONS) {
		__debugbreak();
	}
	_collidingIDs[_countColliding] = otherColliderID;
	++_countColliding;
}

void Collider::Initiate(float radius, ObjectID objectID)
{
	if (radius <= 0) {
		__debugbreak();
	}
	_radius = radius;
	_objectID = objectID;
	Activate();
}

void Collider::Clear()
{
	for (size_t i = 0; i < MAX_SIMULTANEOUS_COLLISIONS; ++i) {
		_collidingIDs[i] = INVALID_COLLIDER_ID;
	}
	_radius = 0;
	_countColliding = 0;
	_objectID = INVALID_OBJECT_ID;
	Deactivate();
}



