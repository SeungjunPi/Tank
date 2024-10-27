#include "Tank.h"

const float VELOCITY_WEIGHT = 0.75f;

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

void Tank::StartMove(EMOVEMENT movement)
{
	switch (movement) {
	case EMOVEMENT::FORWARD:
		_isMovingFoward = true;
		break;
	case EMOVEMENT::BACKWARD:
		_isMovingBackward = true;
		break;
	default:
		__debugbreak();
	}
}

void Tank::EndMove(EMOVEMENT movement)
{
	switch (movement) {
	case EMOVEMENT::FORWARD:
		_isMovingFoward = false;
		break;
	case EMOVEMENT::BACKWARD:
		_isMovingBackward = false;
		break;
	default:
		__debugbreak();
	}
}

void Tank::StartRotate(EROTATION rotation)
{
	switch (rotation) {
	case EROTATION::LEFT:
		_isRotatingLeft = true;
		break;
	case EROTATION::RIGHT:
		_isRotatingRight = true;
		break;
	default:
		__debugbreak();
	}
}

void Tank::EndRotate(EROTATION rotation)
{
	switch (rotation) {
	case EROTATION::LEFT:
		_isRotatingLeft = false;
		break;
	case EROTATION::RIGHT:
		_isRotatingRight = false;
		break;
	default:
		__debugbreak();
	}
}

void Tank::MoveForward(ULONGLONG tickDiff)
{
	_transform.Position.x = _transform.Position.x + _forwardDirection.x * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_transform.Position.y = _transform.Position.y + _forwardDirection.y * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_transform.Position.z = _transform.Position.z + _forwardDirection.z * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_dirty = true;
}

void Tank::MoveBackward(ULONGLONG tickDiff)
{
	_transform.Position.x = _transform.Position.x - _forwardDirection.x * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_transform.Position.y = _transform.Position.y - _forwardDirection.y * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_transform.Position.z = _transform.Position.z - _forwardDirection.z * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_dirty = true;
}

void Tank::RotateRight(ULONGLONG tickDiff)
{
	const float angularVelocity = 3.14159265358979323846f / 1000.f * 60.f / 32.f;
	float radian = tickDiff * angularVelocity;

	_transform.Rotation = Vector4::RotateZP(radian, _transform.Rotation);
	_forwardDirection = Vector3::RotateZP(radian, _forwardDirection);

	_dirty = true;
}

void Tank::RotateLeft(ULONGLONG tickDiff)
{
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
	if (_isMovingFoward) {
		MoveForward(tickDiff);
	}
	if (_isMovingBackward) {
		MoveBackward(tickDiff);
	}
	if (_isRotatingLeft) {
		RotateLeft(tickDiff);
	}
	if (_isRotatingRight) {
		RotateRight(tickDiff);
	}
	return;
}




