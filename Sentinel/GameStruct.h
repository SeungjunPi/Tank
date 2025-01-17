#pragma once

#include "SentinelPch.h"
#include "SentinelAliases.h"
#include "GameMath.h"

struct Chat
{
	UINT32 sender;
	UINT32 length;
	BYTE contents[128];
};

enum EGameObjectType: UINT16
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
