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

UINT16 Collider::GetCollidingIDsAndReset(ColliderID* out)
{
	UINT16 count = _countColliding;
	if (count > 0) {
		memcpy(out, _collidingIDs, sizeof(ColliderID) * count);
		_countColliding = 0;
	}
	return count;
}


void Collider::AddCollidingID(ColliderID otherColliderID)
{
	if (_countColliding == MAX_SIMULTANEOUS_COLLISIONS) {
		__debugbreak();
	}
	_collidingIDs[_countColliding] = otherColliderID;
	++_countColliding;
}

void Collider::Activate(float radius, ObjectID objectID)
{
	if (radius <= 0) {
		__debugbreak();
	}
	_radius = radius;
	_objectID = objectID;
}

void Collider::Deactivate()
{
	for (size_t i = 0; i < MAX_SIMULTANEOUS_COLLISIONS; ++i) {
		_collidingIDs[i] = INVALID_COLLIDER_ID;
	}
	_radius = 0;
	_countColliding = 0;
	_objectID = INVALID_OBJECT_ID;
}



