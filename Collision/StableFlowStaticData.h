#pragma once


#include "CollisionAlias.h"


const ULONGLONG PHYSICS_TICK_RATE = 16;

const ColliderID INVALID_COLLIDER_ID = 0xffff;
const float SAME_POSITION_THRESHOLD = 1E-04f;
const float PENETRATION_THRESHOLD = 1E-02f;


enum ColliderType
{
	COLLIDER_TYPE_SPHERE,
	COLLIDER_TYPE_INVALID
};

struct PhysicalComponent
{
	Transform		transform;
	Vector3			velocity;
	Vector3			angularVelocity;
	ColliderType	colliderType = COLLIDER_TYPE_INVALID;
	float			mass = 0.0f;
	float			radius = 0.0f;
};
