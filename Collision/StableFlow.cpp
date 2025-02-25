
#include "StableFlow.h"
#include "CollisionManager.h"
#include "Collider.h"

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

void StableFlow::ProcessStableFlow(ULONGLONG currentTick)
{
	ULONGLONG tickDiff = currentTick - _lastProcessedTick;
	if (tickDiff < PHYSICS_TICK_RATE) {
		return;
	}
	_lastProcessedTick = currentTick;

	_pCollisionManager->RenewPhysicalComponents();
	_pCollisionManager->DetectCollision();

	// Handle Collision
	JStack* collisionPairs = _pCollisionManager->GetCollisionPairs();
	UINT32 size = collisionPairs->GetCount();
	CollisionPair* pair = (CollisionPair*)collisionPairs->First();
	for (UINT32 i = 0; i < size; ++i) {
		CalculateElasticCollisionNextMovements(pair->a, pair->b, PHYSICS_TICK_RATE);
		ResolvePenetration(pair->a, pair->b);
		++pair;
	}

	// Move Uncollided colliders
	_pCollisionManager->AdvanceFreeBodyMotion();
	

	

}

void StableFlow::CalculateElasticCollisionNextMovements(Collider* a, Collider* b, ULONGLONG tickDiff)
{

}

void StableFlow::ResolvePenetration(Collider* a, Collider* b)
{

}
