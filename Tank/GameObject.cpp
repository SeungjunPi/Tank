#include "GameObject.h"
#include "Collider.h"

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
	return _model.numVertices;
}

void GameObject::SetTransformByServer(const Transform* pTransform)
{
	memcpy(&_transform, pTransform, sizeof(Transform));
	OnUpdateTransform();
	SyncTransformWithCollider();
}

void GameObject::OnRespawn()
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

void GameObject::Tick(ULONGLONG tickDiff)
{
	if (!IsAlive()) {
		return;
	}
	ApplyNextMovement(tickDiff);
}




