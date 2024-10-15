#pragma once


#include "stdafx.h"
#include <cmath>

struct Chat
{
	UINT32 sender;
	UINT32 length;
	BYTE contents[128];
};

enum EGameObjectKind: UINT16
{
	GAME_OBJECT_KIND_TANK = 0x00,
	GAME_OBJECT_KIND_PROJECTILE = 0x01,
	GAME_OBJECT_KIND_OBSTACLE = 0x02
};


struct Vector3;
struct Vector4;
struct Transform;

struct Vector3
{
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	static Vector3 Rotate(Vector3 v, Vector4 quaternion);
	static Vector3 RotateZP(float radian, Vector3 v);
	static Vector3 RotateZM(float radian, Vector3 v);
	static Vector3 RotateZP90(Vector3 v);
	static Vector3 RotateZM90(Vector3 v);
	static float DistanceSquared(Vector3 v, Vector3 w);
};

struct Vector4
{
	float w = 0.f;
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	static Vector4 Product(Vector4 a, Vector4 b);
	static Vector4 RotateZP(float radian, Vector4 v);
	static Vector4 RotateZM(float radian, Vector4 v);
	static Vector4 RotateZP90(Vector4 v);
	static Vector4 RotateZM90(Vector4 v);
};

struct Transform
{
	Vector3 Position;
	Vector4 Rotation;
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

const Vector4 ROTATION_Z_P90 = { 0.5f * sqrtf(2.f), 0.f, 0.f, 0.5f * sqrtf(2.f) };
const Vector4 ROTATION_Z_M90 = { 0.5f * sqrtf(2.f), 0.f, 0.f, -0.5f * sqrtf(2.f) };
