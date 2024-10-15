#include "Tank.h"

Tank::Tank(UINT32 id, UINT32 ownerId)
	: GameObject(id)
	, _ownerId(ownerId)
{
	_forwardDirection = { .0f, -1.0f, .0f };
	_colliderSize = 1;
}

Tank::~Tank()
{
}

void Tank::Initiate(UINT32 id)
{
	_id = id;
	_forwardDirection = { .0f, -1.0f, .0f };
	_transform = { 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f };
	_colliderSize = 1;
	_hitTick = 0;
}

void Tank::Terminate()
{
	_id = 0;
	_forwardDirection = { 0, };
	_transform = { 0, };
	_colliderSize = 0;
}

void Tank::MoveForward(ULONGLONG tickDiff)
{
	_transform.Position.x = _transform.Position.x + _forwardDirection.x * (tickDiff / 1000.f * 60.f);
	_transform.Position.y = _transform.Position.y + _forwardDirection.y * (tickDiff / 1000.f * 60.f);
	_transform.Position.z = _transform.Position.z + _forwardDirection.z * (tickDiff / 1000.f * 60.f);
	_dirty = true;
}

void Tank::MoveBackward(ULONGLONG tickDiff)
{
	_transform.Position.x = _transform.Position.x - _forwardDirection.x * (tickDiff / 1000.f * 60.f);
	_transform.Position.y = _transform.Position.y - _forwardDirection.y * (tickDiff / 1000.f * 60.f);
	_transform.Position.z = _transform.Position.z - _forwardDirection.z * (tickDiff / 1000.f * 60.f);
	_dirty = true;
}

void Tank::RotateRight(ULONGLONG tickDiff)
{
	/*_transform.Rotation = Vector4::Product(ROTATION_Z_P90, _transform.Rotation);
	_forwardDirection = Vector3::RotateZP90(_forwardDirection);*/
	const float angularVelocity = 3.14159265358979323846f / 1000.f * 60.f / 32.f;
	float radian = tickDiff * angularVelocity;

	_transform.Rotation = Vector4::RotateZP(radian, _transform.Rotation);
	_forwardDirection = Vector3::RotateZP(radian, _forwardDirection);

	_dirty = true;
}

void Tank::RotateLeft(ULONGLONG tickDiff)
{
	/*_transform.Rotation = Vector4::Product(ROTATION_Z_M90, _transform.Rotation);
	_forwardDirection = Vector3::RotateZM90(_forwardDirection);*/

	const float angularVelocity = 3.14159265358979323846f / 1000.f * 60.f / 32.f;
	float radian = tickDiff * angularVelocity;

	_transform.Rotation = Vector4::RotateZM(radian, _transform.Rotation);
	_forwardDirection = Vector3::RotateZM(radian, _forwardDirection);


	_dirty = true;
}

void Tank::GetTurretInfo(Vector3* out_position, Vector3* out_direction) const
{
	Vector3 position = _transform.Position;
	Vector4 rotation = _transform.Rotation;

	Vector3 v = { 0, -1.f, 0 };
	v = Vector3::Rotate(v, rotation);
	v.x += position.x;
	v.y += position.y;
	v.z += position.z;
	*out_position = v;

	memcpy(out_direction, &_forwardDirection, sizeof(Vector3));
}

UINT32 Tank::GetOwnerId() const
{
	return _ownerId;
}

void Tank::OnFrame(ULONGLONG tickDiff)
{
	return;
}




