#pragma once


#include "SentinelPch.h"
#include "SentinelAliases.h"
#include <cmath>

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


struct Vector3;
struct Quaternion;
struct Transform;

struct Vector3
{
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	static Vector3 Rotate(Vector3 v, Quaternion quaternion);
	static Vector3 RotateZP(float radian, Vector3 v);
	static Vector3 RotateZM(float radian, Vector3 v);
	static Vector3 RotateZP90(Vector3 v);
	static Vector3 RotateZM90(Vector3 v);
	static float DistanceSquared(Vector3 v, Vector3 w);
};

struct Quaternion
{
	float w = 0.f;
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	static Quaternion Product(Quaternion a, Quaternion b);
	static Quaternion RotateZP(float radian, Quaternion v);
	static Quaternion RotateZM(float radian, Quaternion v);
	static Quaternion RotateZP90(Quaternion v);
	static Quaternion RotateZM90(Quaternion v);

	static float AngularDistance(Quaternion q1, Quaternion q2);
};

struct Transform
{
	Vector3 Position;
	Quaternion Rotation;
};

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

const Quaternion ROTATION_Z_P90 = { 0.5f * sqrtf(2.f), 0.f, 0.f, 0.5f * sqrtf(2.f) };
const Quaternion ROTATION_Z_M90 = { 0.5f * sqrtf(2.f), 0.f, 0.f, -0.5f * sqrtf(2.f) };
