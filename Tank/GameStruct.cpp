#include "GameStruct.h"

Vector4 Vector4::Product(Vector4 a, Vector4 b)
{
    Vector4 result = { 0, };
    result.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
    result.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
    result.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
    result.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;

    return result;
}

Vector4 Vector4::RotateZP(float radian, Vector4 v)
{
    float halfAngle = radian * 0.5f;
    Vector4 w = { cosf(halfAngle), 0, 0, sinf(halfAngle) };
    Vector4 rotV = Product(w, v);
    return rotV;
}

Vector4 Vector4::RotateZM(float radian, Vector4 v)
{
    float halfAngle = radian * 0.5f;
    Vector4 w = { cosf(halfAngle), 0, 0, -sinf(halfAngle) };
    Vector4 rotV = Product(w, v);
    return rotV;
}

Vector4 Vector4::RotateZP90(Vector4 v)
{
    Vector4 q = ROTATION_Z_P90;
    Vector4 q_ = q;
    q_.x *= -1;
    q_.y *= -1;
    q_.z *= -1;

    Vector4 w = Product(q, v);
    w = Product(w, q_);

    return w;
}

Vector4 Vector4::RotateZM90(Vector4 v)
{
    Vector4 q = ROTATION_Z_M90;
    Vector4 q_ = q;
    q_.x *= -1;
    q_.y *= -1;
    q_.z *= -1;

    Vector4 w = Product(q, v);
    w = Product(w, q_);
    return w;
}

Vector3 Vector3::Rotate(Vector3 v, Vector4 rotQuat)
{
    Vector4 quaternionV;
    quaternionV.x = v.x;
    quaternionV.y = v.y;
    quaternionV.z = v.z;
    quaternionV.w = 0;

    Vector4 w = Vector4::Product(rotQuat, quaternionV);
    rotQuat.x *= -1;
    rotQuat.y *= -1;
    rotQuat.z *= -1;

    w = Vector4::Product(w, rotQuat);

    Vector3 ret;
    ret.x = w.x;
    ret.y = w.y;
    ret.z = w.z;

    return ret;
}

Vector3 Vector3::RotateZP(float radian, Vector3 v)
{
    float halfAngle = radian * 0.5f;
    Vector4 w = { cosf(halfAngle), 0, 0, sinf(halfAngle) };

    Vector3 rotV = Vector3::Rotate(v, w);
    return rotV;
}

Vector3 Vector3::RotateZM(float radian, Vector3 v)
{
    float halfAngle = radian * 0.5f;
    Vector4 w = { cosf(halfAngle), 0, 0, -sinf(halfAngle) };

    Vector3 rotV = Vector3::Rotate(v, w);
    return rotV;
}

Vector3 Vector3::RotateZP90(Vector3 v)
{
    Vector4 quaternionV;
    quaternionV.x = v.x;
    quaternionV.y = v.y;
    quaternionV.z = v.z;
    quaternionV.w = 0;

    Vector4 quaternionW = Vector4::RotateZP90(quaternionV);

    Vector3 w;
    w.x = quaternionW.x;
    w.y = quaternionW.y;
    w.z = quaternionW.z;

    return w;
}

Vector3 Vector3::RotateZM90(Vector3 v)
{
    Vector4 quaternionV;
    quaternionV.x = v.x;
    quaternionV.y = v.y;
    quaternionV.z = v.z;
    quaternionV.w = 0;

    Vector4 quaternionW = Vector4::RotateZM90(quaternionV);

    Vector3 w;
    w.x = quaternionW.x;
    w.y = quaternionW.y;
    w.z = quaternionW.z;

    return w;
}

float Vector3::DistanceSquared(Vector3 v, Vector3 w)
{
    float x = v.x - w.x;
    float y = v.y - w.y;
    float z = v.z - w.z;
    return x * x + y * y + z * z;
}
