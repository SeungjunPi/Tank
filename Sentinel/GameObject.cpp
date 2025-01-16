#include "GameObject.h"

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
	_dirty = true;
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

BOOL GameObject::IsDirty()
{
	return _dirty;
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
	OnUpdateTransform();
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

BOOL GameObject::IsTransformCloseEnough(const Transform* other)
{
	if (memcmp(&_transform, other, sizeof(Transform)) == 0) {
		return true;
	}
	
	return false;
}

