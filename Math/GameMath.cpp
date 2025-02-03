#include "GameMath.h"
#include <math.h>

void Quaternion::Normalize(Quaternion* out_normalized, Quaternion q)
{
    float norm = sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
    if (norm == 0.0f) {
        __debugbreak();
    }
    out_normalized->w = q.w / norm;
    out_normalized->x = q.x / norm;
    out_normalized->y = q.y / norm;
    out_normalized->z = q.z / norm;
}

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
    float dotProduct = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;

    if (dotProduct < 0.0f) {
        dotProduct = -dotProduct;
    }

    dotProduct = fmaxf(fminf(dotProduct, 1.0f), 0.0f);
    return 2.0f * acosf(fabsf(dotProduct));
}

Vector3 Vector3::operator*(const float scalar)
{
    return Vector3{x * scalar, y * scalar, z * scalar};
}

Vector3 Vector3::operator+(const float scalar)
{
    return Vector3{ x + scalar, y + scalar, z + scalar };
}

Vector3 Vector3::operator-(const float scalar)
{
    return Vector3{ x - scalar, y - scalar, z - scalar };
}

Vector3 Vector3::operator+(const Vector3& another)
{
    return Vector3{ x + another.x, y + another.y, z + another.z };
}

Vector3 Vector3::operator+(const Vector3&& another)
{
    return Vector3{ x + another.x, y + another.y, z + another.z };
}

Vector3 Vector3::operator-(const Vector3& another)
{
    return Vector3{ x - another.x, y - another.y, z - another.z }; 
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

float Vector3::SquareSum(Vector3 v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

float Vector3::Norm(Vector3 v)
{
    float sqSum = SquareSum(v);
    return sqrtf(sqSum);
}

void Vector3::Normalize(Vector3* out_normalized, Vector3 v)
{
    float norm = Norm(v);

    if (norm < 0E-010) {
        __debugbreak();
    }

    out_normalized->x = v.x / norm;
    out_normalized->y = v.y / norm;
    out_normalized->z = v.z / norm;
}

float Vector3::DistanceSquared(Vector3 v, Vector3 w)
{
    float x = v.x - w.x;
    float y = v.y - w.y;
    float z = v.z - w.z;
    return x * x + y * y + z * z;
}

float Vector3::DotProduct(Vector3 v, Vector3 w)
{
    return v.x * w.x + v.y * w.y + v.z * w.z;
}

const float Vector2::TRUNCATION = 1E-02f;

float Vector2::SquareSum(Vector2 v)
{
    return v.x * v.x + v.y * v.y;
}

float Vector2::Norm(Vector2 v)
{
    return sqrtf(SquareSum(v));
}

void Vector2::Normalize(Vector2* out_normalized, Vector2 v)
{
    float norm = Norm(v);
    out_normalized->x = v.x / norm;
    out_normalized->y = v.y / norm;
}

float Vector2::DistanceSquared(Vector2 v, Vector2 w)
{
    float dx = v.x - w.x;
    float dy = v.y - w.y;

    return dx * dx + dy * dy;
}

int Vector2::DeterminRotationDirection(Vector2 crntDir, Vector2 targetDir)
{
    float dot = DotProduct(crntDir, targetDir);
    dot /= Norm(crntDir);
    dot /= Norm(targetDir);
    if (dot >= 1.0 - TRUNCATION) {
        // Same Direction
        return 0;
    }

    float cross = CrossProduct(crntDir, targetDir);
    if (cross >= 0) {
        return 1;
    }
    else {
        return -1;
    }
}

float Vector2::DotProduct(Vector2 v, Vector2 w)
{
    return v.x * w.x + v.y * w.y;
}

float Vector2::CrossProduct(Vector2 v, Vector2 w)
{
    return v.x * w.y - v.y * w.x;
}
