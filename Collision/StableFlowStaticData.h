#pragma once


#include "CollisionAlias.h"


const ULONGLONG PHYSICS_TICK_RATE = 16;

const ColliderID INVALID_COLLIDER_ID = 0xffff;


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
