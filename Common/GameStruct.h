#pragma once

#include "pch.h"
#include "GameMath.h"


using ObjectKey = UINT16; // EGameObjectType의 크기를 변경한다면 여기를 반드시 변경해줘야 함.


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



enum EColliderKindnessFlag : UINT32
{
	COLLIDER_KINDNESS_TANK = 0b1,
	COLLIDER_KINDNESS_PROJECTILE = 0b10,
	COLLIDER_KINDNESS_OBSTACLE = 0b100,
	COLLIDER_KINDNESS_SAME_POSITION = ((UINT32)0b1 << 31)
};

const static ULONGLONG OBJECT_DESTROY_DELAY = 0x1000;

const float TANK_TRANSLATION_SPEED = 0.5f / 60.f;
const float TANK_ROTATION_SPEED = 3.14159265358979323846f / 1000.f * 60.f / 32.f * 0.25f;

const float TANK_COLLIDER_RADIUS = 2.0f;
const float TANK_COLLIDER_MASS = 10.f;
const ULONGLONG TANK_MACHINE_GUN_DELAY = 250;

const float PROJECTILE_COLLIDER_RADIUS = 1.f;
const float PROJECTILE_COLLIDER_MASS = 0.1f;
const float PROJECTILE_TRANSLATION_SPEED = 40.0f / 1000.0f;

struct TankScore
{
	UINT16 hitCount = 0;
	UINT16 killCount = 0;
	UINT16 deathCount = 0;
};

using PlayerInputState = UINT32;
const PlayerInputState PLAYER_INPUT_NONE = 0;
const PlayerInputState PLAYER_INPUT_FLAG_UP = (UINT32)0b1;
const PlayerInputState PLAYER_INPUT_FLAG_DOWN = (UINT32)0b10;
const PlayerInputState PLAYER_INPUT_FLAG_LEFT = (UINT32)0b100;
const PlayerInputState PLAYER_INPUT_FLAG_RIGHT = (UINT32)0b1000;
const PlayerInputState PLAYER_INPUT_FLAG_FIRE_MACHINE_GUN = (UINT32)0b100000;
const PlayerInputState PLAYER_INPUT_MOVE_FLAGS = (UINT32)0b1111;