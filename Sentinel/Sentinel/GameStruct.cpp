#include <cmath>
#include "GameStruct.h"


Quaternion Quaternion::Product(Quaternion a, Quaternion b)
{
    Quaternion result = { 0, };
    result.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
    result.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
    result.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
    result.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;

    return result;
}

Quaternion Quaternion::RotateZP(float radian, Quaternion v)
{
    float halfAngle = radian * 0.5f;
    Quaternion w = { cosf(halfAngle), 0, 0, sinf(halfAngle) };
    Quaternion rotV = Product(w, v);
    return rotV;
}

Quaternion Quaternion::RotateZM(float radian, Quaternion v)
{
    float halfAngle = radian * 0.5f;
    Quaternion w = { cosf(halfAngle), 0, 0, -sinf(halfAngle) };
    Quaternion rotV = Product(w, v);
    return rotV;
}

Quaternion Quaternion::RotateZP90(Quaternion v)
{
    Quaternion q = ROTATION_Z_P90;
    Quaternion q_ = q;
    q_.x *= -1;
    q_.y *= -1;
    q_.z *= -1;

    Quaternion w = Product(q, v);
    w = Product(w, q_);

    return w;
}

Quaternion Quaternion::RotateZM90(Quaternion v)
{
    Quaternion q = ROTATION_Z_M90;
    Quaternion q_ = q;
    q_.x *= -1;
    q_.y *= -1;
    q_.z *= -1;

    Quaternion w = Product(q, v);
    w = Product(w, q_);
    return w;
}

float Quaternion::AngularDistance(Quaternion q1, Quaternion q2)
{
    float dotProduct = q1.w* q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
    return 2.0 * acosf(fabs(dotProduct));
}

Vector3 Vector3::Rotate(Vector3 v, Quaternion rotQuat)
{
    Quaternion quaternionV;
    quaternionV.x = v.x;
    quaternionV.y = v.y;
    quaternionV.z = v.z;
    quaternionV.w = 0;

    Quaternion w = Quaternion::Product(rotQuat, quaternionV);
    rotQuat.x *= -1;
    rotQuat.y *= -1;
    rotQuat.z *= -1;

    w = Quaternion::Product(w, rotQuat);

    Vector3 ret;
    ret.x = w.x;
    ret.y = w.y;
    ret.z = w.z;

    return ret;
}

Vector3 Vector3::RotateZP(float radian, Vector3 v)
{
    float halfAngle = radian * 0.5f;
    Quaternion w = { cosf(halfAngle), 0, 0, sinf(halfAngle) };

    Vector3 rotV = Vector3::Rotate(v, w);
    return rotV;
}

Vector3 Vector3::RotateZM(float radian, Vector3 v)
{
    float halfAngle = radian * 0.5f;
    Quaternion w = { cosf(halfAngle), 0, 0, -sinf(halfAngle) };

    Vector3 rotV = Vector3::Rotate(v, w);
    return rotV;
}

Vector3 Vector3::RotateZP90(Vector3 v)
{
    Quaternion quaternionV;
    quaternionV.x = v.x;
    quaternionV.y = v.y;
    quaternionV.z = v.z;
    quaternionV.w = 0;

    Quaternion quaternionW = Quaternion::RotateZP90(quaternionV);

    Vector3 w;
    w.x = quaternionW.x;
    w.y = quaternionW.y;
    w.z = quaternionW.z;

    return w;
}

Vector3 Vector3::RotateZM90(Vector3 v)
{
    Quaternion quaternionV;
    quaternionV.x = v.x;
    quaternionV.y = v.y;
    quaternionV.z = v.z;
    quaternionV.w = 0;

    Quaternion quaternionW = Quaternion::RotateZM90(quaternionV);

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
