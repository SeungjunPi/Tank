
#pragma once

#include <math.h>

struct Vector3;
struct Quaternion;
struct Transform;

const float EPSILON = 1E-05;

struct Vector2
{
	float x = 0.f;
	float y = 0.f;

	static const float TRUNCATION;

	static float SquareSum(Vector2 v);
	static float Norm(Vector2 v);

	static void Normalize(Vector2* out_normalized, Vector2 v);

	static float DistanceSquared(Vector2 v, Vector2 w);

	static int DeterminRotationDirection(Vector2 crntDir, Vector2 targetDir);

	static float DotProduct(Vector2 v, Vector2 w);
	static float CrossProduct(Vector2 v, Vector2 w);
};

struct Vector3
{
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	Vector3 operator*(const float scalar);
	Vector3 operator+(const float scalar);
	Vector3 operator-(const float scalar);
	Vector3 operator+(const Vector3& another);
	Vector3 operator+(const Vector3&& another);
	Vector3 operator-(const Vector3& another);

	static Vector3 Rotate(Vector3 v, Quaternion quaternion);
	static Vector3 RotateZP(float radian, Vector3 v);
	static Vector3 RotateZM(float radian, Vector3 v);
	static Vector3 RotateZP90(Vector3 v);
	static Vector3 RotateZM90(Vector3 v);

	static float SquareSum(Vector3 v);
	static float Norm(Vector3 v);
	static void Normalize(Vector3* out_normalized, Vector3 v);

	static float DistanceSquared(Vector3 v, Vector3 w);
	static float DotProduct(Vector3 v, Vector3 w);
};

struct Quaternion
{
	float w = 1.f;
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	static void Normalize(Quaternion* out_normalized, Quaternion q);

	static Quaternion Product(Quaternion a, Quaternion b);
	static Quaternion RotateZP(float radian, Quaternion v);
	static Quaternion RotateZM(float radian, Quaternion v);
	static Quaternion RotateZP90(Quaternion v);
	static Quaternion RotateZM90(Quaternion v);
	static Quaternion ConvertFromAngle(Vector3 v);

	static float AngularDistance(Quaternion q1, Quaternion q2);
};

struct Transform
{
	Vector3 Position;
	Quaternion Rotation;
};

const Quaternion ROTATION_Z_P90 = { 0.5f * sqrtf(2.f), 0.f, 0.f, 0.5f * sqrtf(2.f) };
const Quaternion ROTATION_Z_M90 = { 0.5f * sqrtf(2.f), 0.f, 0.f, -0.5f * sqrtf(2.f) };