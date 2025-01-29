#pragma once

#include "SentinelPch.h"

enum EColliderKindnessFlag: UINT32
{
	COLLIDER_KINDNESS_TANK = 0b1,
	COLLIDER_KINDNESS_PROJECTILE = 0b10,
	COLLIDER_KINDNESS_OBSTACLE = 0b100,
};
