#pragma once

#include "TankPch.h"
#include "GameMath.h"

enum EGameObjectType : UINT16
{
	GAME_OBJECT_TYPE_TANK = 0x00,
	GAME_OBJECT_TYPE_PROJECTILE = 0x01,
	GAME_OBJECT_TYPE_OBSTACLE = 0x02,
	GAME_OBJECT_TYPE_MAX // invalid type number
};

struct ObjectID
{
	EGameObjectType type;
	ObjectKey key;

	bool equals(const ObjectID& other) const {
		return type == other.type && key == other.key;
	}
};

const UINT16 INVALID_OBJECT_KEY = 0xFFFF;
const ObjectID INVALID_OBJECT_ID = { GAME_OBJECT_TYPE_MAX, INVALID_OBJECT_KEY };

struct Vertex
{
	Vector3 v;
	WCHAR c;
};

struct Model
{
	Vertex* vertices;
	UINT numVertices;
};

enum EMovementEventFlag : UINT64
{
	MOVEMENT_EVENT_FLAG_UP = 0b00000001,
	MOVEMENT_EVENT_FLAG_DOWN = 0b00000010,
	MOVEMENT_EVENT_FLAG_LEFT = 0b00000100,
	MOVEMENT_EVENT_FLAG_RIGHT = 0b00001000,
	MOVEMENT_EVENT_FLAG_FIRE = 0b00100000
};

const UINT64 MOVEMENT_FLAGS = 0b1111;
