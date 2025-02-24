
#include "StableFlow.h"
#include "CollisionManager.h"

void CreateStableFlow(IStableFlow** dst)
{
	*dst = new StableFlow();
}

void DeleteStableFlow(IStableFlow* ptr)
{
	delete ptr;
}


StableFlow::StableFlow()
{
	_pCollisionManager = new CollisionManager();
}

StableFlow::~StableFlow()
{
	delete _pCollisionManager;
}

Collider* StableFlow::GetNewColliderPtr(float radius, GameObject* pObj, const Vector3* center, UINT32 kindness)
{
	Collider* pCollider = _pCollisionManager->GetNewColliderPtr(radius, pObj, center, kindness);
	return pCollider;
}

Collider* StableFlow::GetAttachedColliderPtr(ColliderID id)
{
	Collider* pCollider = _pCollisionManager->GetAttachedColliderPtr(id);
	return pCollider;
}

void StableFlow::ReturnCollider(Collider* pColloder)
{
	_pCollisionManager->ReturnCollider(pColloder);
}


JStack* StableFlow::GetCollisionPairs()
{
	JStack* pairs = _pCollisionManager->GetCollisionPairs();
	return pairs;
}

void StableFlow::ProcessStableFlow()
{
	_pCollisionManager->RenewPhysicalComponets();
	
	_pCollisionManager->DetectCollision();
	
	// Calculate Physics and store result to collider

}
