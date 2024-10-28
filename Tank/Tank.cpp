#include "Tank.h"
#include "StaticData.h"

const float VELOCITY_WEIGHT = 0.75f;

Tank::Tank(UINT32 id)
	: GameObject(id)
{
	_forwardDirection = { .0f, -1.0f, .0f };

	_model = g_pTankModel;
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
	_model = g_pTankModel;
	_colliderSize = 1;
	_hitTick = 0;
}

void Tank::Terminate()
{
	_id = 0;
	_forwardDirection = { 0, };
	_transform = { 0, };
	_model = { nullptr, 0 };
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

void Tank::EndMove(EMOVEMENT movement, const Transform* pTransform)
{
	switch (movement) {
	case EMOVEMENT::FORWARD:
		_isMovingFoward = false;
		memcpy(&_transform, pTransform, sizeof(Transform));
		break;
	case EMOVEMENT::BACKWARD:
		_isMovingBackward = false;
		memcpy(&_transform, pTransform, sizeof(Transform));
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

void Tank::EndRotate(EROTATION rotation, const Transform* pTransform)
{
	switch (rotation) {
	case EROTATION::LEFT:
		memcpy(&_transform, pTransform, sizeof(Transform));
		_isRotatingLeft = false;
		break;
	case EROTATION::RIGHT:
		memcpy(&_transform, pTransform, sizeof(Transform));
		_isRotatingRight = false;
		break;
	default:
		__debugbreak();
	}
}

void Tank::MoveForward(ULONGLONG tickDiff)
{
	_forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
	_transform.Position.x = _transform.Position.x + _forwardDirection.x * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_transform.Position.y = _transform.Position.y + _forwardDirection.y * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_transform.Position.z = _transform.Position.z + _forwardDirection.z * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_dirty = true;
}

void Tank::MoveBackward(ULONGLONG tickDiff)
{
	_forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);
	_transform.Position.x = _transform.Position.x - _forwardDirection.x * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_transform.Position.y = _transform.Position.y - _forwardDirection.y * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_transform.Position.z = _transform.Position.z - _forwardDirection.z * (tickDiff / 1000.f * 60.f) * VELOCITY_WEIGHT;
	_dirty = true;
}

void Tank::RotateRight(ULONGLONG tickDiff)
{
	const float angularVelocity = 3.14159265358979323846f / 1000.f * 60.f / 32.f;
	float radian = tickDiff * angularVelocity * VELOCITY_WEIGHT;

	_transform.Rotation = Quaternion::RotateZP(radian, _transform.Rotation);
	
	_dirty = true;
}

void Tank::RotateLeft(ULONGLONG tickDiff)
{
	const float angularVelocity = 3.14159265358979323846f / 1000.f * 60.f / 32.f;
	float radian = tickDiff * angularVelocity * VELOCITY_WEIGHT;

	_transform.Rotation = Quaternion::RotateZM(radian, _transform.Rotation);

	_dirty = true;
}

void Tank::GetTurretInfo(Vector3* out_position, Vector3* out_direction) const
{
	Vector3 position = _transform.Position;
	Quaternion rotation = _transform.Rotation;

	Vector3 v = _model.vertices[0].v;
	v = Vector3::Rotate(v, rotation);
	v.x += position.x;
	v.y += position.y;
	v.z += position.z;
	*out_position = v;

	const Vector3 forwardDirection = Vector3::Rotate(FORWARD_DIRECTION, _transform.Rotation);

	memcpy(out_direction, &forwardDirection, sizeof(Vector3));
}

void Tank::GetTurretInfo(Transform* out_transform) const
{
	Quaternion rotation = _transform.Rotation;

	Vector3 v = _model.vertices[0].v;
	v = Vector3::Rotate(v, rotation);
	v.x += _transform.Position.x;
	v.y += _transform.Position.y;
	v.z += _transform.Position.z;

	out_transform->Position = v;
	out_transform->Rotation = _transform.Rotation;
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


