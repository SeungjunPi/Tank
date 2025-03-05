#include "GameObject.h"
#include "Collider.h"

GameObject::GameObject()
	: _model{ nullptr, 0 }
{
}

GameObject::GameObject(ObjectID id, UserDBIndex ownerID)
	: _id(id)
	, _model{ nullptr, 0 }
	, _ownerID(ownerID)
{
}

GameObject::GameObject(ObjectID id, UserDBIndex ownerID, BOOL activatable)
	: _id(id)
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
	return _physicalComponent.transform;
}

UserDBIndex GameObject::GetOwnerID() const
{
	return _ownerID;
}

const Transform* GameObject::GetTransformPtr() const
{
	return &_physicalComponent.transform;
}

Vector3 GameObject::GetPosition() const
{
	return _physicalComponent.transform.Position;
}


UINT GameObject::GetTransformedModel(Vertex* out_vertices)
{
	Vector3 position = _physicalComponent.transform.Position;
	Quaternion rotation = _physicalComponent.transform.Rotation;
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

void GameObject::ResetDynamicPhysicalComponent()
{
	_physicalComponent.transform = Transform();
	_physicalComponent.velocity = Vector3();
	_physicalComponent.angularVelocity = Vector3();
}

void GameObject::SetTransformByServer(const Transform* pTransform)
{
	memcpy(&_physicalComponent.transform, pTransform, sizeof(Transform));
	OnUpdateTransform();
}

void GameObject::OnRespawn()
{
	if (!_isActivatable) {
		__debugbreak();
	}
	_isAlive = true;
	_physicalComponent.transform = { 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f };
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

void GameObject::ApplyStableFlowResult(ULONGLONG tickDiff)
{
	_pCollider->OverwriteComputedResultsToGameObject();
	OnUpdateTransform();
}

void GameObject::Tick(ULONGLONG tickDiff)
{
	if (!IsAlive()) {
		return;
	}
	ApplyStableFlowResult(tickDiff);
}




