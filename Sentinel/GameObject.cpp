#include "GameObject.h"
#include "Global.h"
#include "Collider.h"

GameObject::GameObject()
	: _transform{ 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f }
{
}

GameObject::GameObject(ObjectID id, UserDBIndex ownerId)
	: _transform{ 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f }
	, _id(id)
	, _ownerIndex(ownerId)
{
}

GameObject::GameObject(ObjectID id, UserDBIndex ownerId, BOOL activatable)
	: _transform{ 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f }
	, _id(id)
	, _ownerIndex(ownerId)
	, _isActivatable(activatable)
{
}

GameObject::~GameObject()
{
}

ObjectID GameObject::GetID() const
{
	return _id;
}

Transform GameObject::GetTransform() const
{
	return _transform;
}

const Transform* GameObject::GetTransformPtr() const
{
	return &_transform;
}

Vector3 GameObject::GetPosition() const
{
	return _transform.Position;
}

BOOL GameObject::UpdateFrom(const GameObject* pOther)
{
	if (_id.equals(pOther->_id)) {
		__debugbreak();
		return false;
	}

	memcpy(&_transform, &pOther->_transform, sizeof(Transform));
	return true;
}

void GameObject::UpdateTransform(const Transform* pTransform)
{
	memcpy(&_transform, pTransform, sizeof(Transform));
}

BOOL GameObject::UpdateTransformIfValid(const Transform* pTransform)
{
	BOOL isClosed = IsTransformCloseEnough(pTransform);
	if (isClosed) {
		UpdateTransform(pTransform);
		return true;
	}
	
	return false;
	
}

void GameObject::SetPosition(Vector3 position)
{
	_transform.Position.x = position.x;
	_transform.Position.y = position.y;
	_transform.Position.z = position.z;
}

UserDBIndex GameObject::GetOwnerId() const
{
	return _ownerIndex;
}

void GameObject::OnRespawn()
{
	if (!_isActivatable) {
		__debugbreak();
	}
	_isAlive = true;
	_transform = { 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f };
}

BOOL GameObject::IsDestroyed(ULONGLONG currentTick) const
{
	return !_isActivatable && (!_isAlive);
}

void GameObject::AttachCollider(Collider* pCollider)
{
	assert(_pCollider == nullptr);

	_pCollider = pCollider;
}

void GameObject::SyncTransformWithCollider()
{
	if (_pCollider != nullptr) {
		_pCollider->UpdateCenter(_transform.Position);
	}
}

Vector3 GameObject::GetMovementVector(ULONGLONG tickDiff)
{
	return _movementDirection * _movementSpeed * tickDiff;
}

Quaternion GameObject::GetDeltaRotation(ULONGLONG tickDiff)
{
	// 기본 회전
	return Quaternion::RotateZP(_rotationSpeed * tickDiff, _rotationDirection);
}

void GameObject::UpdateTransform(ULONGLONG tickDiff)
{
	Vector3 movement = GetMovementVector(tickDiff);
	_transform.Position = _transform.Position + movement;

	Quaternion quat = GetDeltaRotation(tickDiff);
	_transform.Rotation = Quaternion::Product(_transform.Rotation, quat);
}

BOOL GameObject::IsTransformCloseEnough(const Transform* other)
{
	if (memcmp(&_transform, other, sizeof(Transform)) == 0) {
		return true;
	}
	
	return false;
}

