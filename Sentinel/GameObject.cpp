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

BOOL GameObject::UpdateTransformIfValid(const Transform* pTransform)
{
	BOOL isClosed = IsTransformCloseEnough(pTransform);
	if (isClosed) {
		memcpy(&_transform, pTransform, sizeof(Transform));
		SyncTransformWithCollider();
		return true;
	}
	
	return false;
	
}

void GameObject::Tick(ULONGLONG tickDiff)
{
	if (!IsAlive()) {
		return;
	}

	ApplyNextMovement(tickDiff);
}

UserDBIndex GameObject::GetOwnerId() const
{
	return _ownerIndex;
}

void GameObject::Respawn()
{
	if (!_isActivatable) {
		__debugbreak();
	}
	_isAlive = true;
	_transform = { 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f };
	_hitTick = 0;
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

void GameObject::ApplyNextMovement(ULONGLONG tickDiff)
{
	_transform.Position = _nextPosition;
	_transform.Rotation = Quaternion::RotateZP(_nextRotationAngle, _transform.Rotation);

	// printf("P[%f, %f, %f], R[%f, %f, %f, %f]\n", _transform.Position.x, _transform.Position.y, _transform.Position.z, _transform.Rotation.w, _transform.Rotation.x, _transform.Rotation.y, _transform.Rotation.z);

	OnUpdateTransform();
	SyncTransformWithCollider();
}

BOOL GameObject::IsTransformCloseEnough(const Transform* other)
{
	if (memcmp(&_transform, other, sizeof(Transform)) == 0) {
		return true;
	}
	
	return false;
}

