#include "Collider.h"

Collider::Collider()
{
}

void Collider::ResetCollisionFlag()
{
	_collisionKindnessFlag = 0;
}

void Collider::UpdatePhysicsComponentFromGameObject()
{
	memcpy(&_physicalComponent.transform, &_pObjectPhysicalComponent->transform, sizeof(Transform));
	memcpy(&_physicalComponent.velocity, &_pObjectPhysicalComponent->velocity, sizeof(Transform));
	memcpy(&_physicalComponent.angularVelocity, &_pObjectPhysicalComponent->angularVelocity, sizeof(Transform));
}

void Collider::OverwriteComputedResultsToGameObject()
{
	memcpy(&_pObjectPhysicalComponent->transform, &_nextTransform, sizeof(Transform));
	memcpy(&_pObjectPhysicalComponent->velocity, &_nextVelocity, sizeof(Vector3));
	memcpy(&_pObjectPhysicalComponent->angularVelocity, &_nextAngularVelocity, sizeof(Vector3));
}

void Collider::GetComputedResults(Transform* out_transform, Vector3* out_velocity, Vector3* out_angularVelocity) const
{
	memcpy(out_transform, &_nextTransform, sizeof(Transform));
	memcpy(out_velocity, &_nextVelocity, sizeof(Vector3));
	memcpy(out_angularVelocity, &_nextAngularVelocity, sizeof(Vector3));
}

void Collider::Initiate(GameObject* pObj, PhysicalComponent* objectPhysicalComponent)
{
	memcpy(&_physicalComponent, objectPhysicalComponent, sizeof(PhysicalComponent));
	_pObject = pObj;
	_pObjectPhysicalComponent = objectPhysicalComponent;
	Activate();
}

void Collider::Clear()
{
	_isActive = FALSE;
	_collisionKindnessFlag = 0;
	_pObject = nullptr;
	_pObjectPhysicalComponent = nullptr;

	_physicalComponent = PhysicalComponent();
	
	Deactivate();
}




