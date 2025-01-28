#include "GameObject.h"

GameObject::GameObject()
	: _transform{ 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f }
	, _model{ nullptr, 0 }
{
}

GameObject::GameObject(ObjectID id, UserDBIndex ownerID)
	: _transform{ 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f }
	, _id(id)
	, _model{ nullptr, 0 }
	, _ownerID(ownerID)
{
}

GameObject::GameObject(ObjectID id, UserDBIndex ownerID, BOOL activatable)
	: _transform{ 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f }
	, _id(id)
	, _isActivatable(activatable)
	, _model{ nullptr, 0 }
	, _ownerID(ownerID)
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

UserDBIndex GameObject::GetOwnerID() const
{
	return _ownerID;
}

const Transform* GameObject::GetTransformPtr() const
{
	return &_transform;
}

Vector3 GameObject::GetPosition() const
{
	return _transform.Position;
}


UINT GameObject::GetTransformedModel(Vertex* out_vertices)
{
	Vector3 position = _transform.Position;
	Quaternion rotation = _transform.Rotation;
	for (UINT i = 0; i < _model.numVertices; ++i) {
		Vector3 v = _model.vertices[i].v;

		v = Vector3::Rotate(v, rotation);
		v.x += position.x;
		v.y += position.y;
		v.z += position.z;
		out_vertices[i].v = v;
		out_vertices[i].c = _model.vertices[i].c;
	}
	_dirty = false;
	return _model.numVertices;
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

void GameObject::OnHit(ULONGLONG currentTick)
{
	if (_hitTick == 0) {
 		_hitTick = currentTick;
	}
}

void GameObject::OnRespawn()
{
	if (!_isActivatable) {
		__debugbreak();
	}
	_isAlive = true;
	_transform = { 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f };
	_hitTick = 0;
	OnUpdateTransform();
}

BOOL GameObject::IsDestroyed(ULONGLONG currentTick) const
{
	return !_isActivatable && (!_isAlive || _hitTick != 0);
}

void GameObject::Respawn()
{
	OnRespawn();
}

void GameObject::AttachCollider(Collider* pCollider)
{
	if (_pCollider != nullptr) {
		__debugbreak();
	}

	_pCollider = pCollider;
}

