
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

Collider* StableFlow::GetNewColliderPtr(GameObject* pObj, PhysicalComponent* pObjPhyComponent)
{
	Collider* pCollider = _pCollisionManager->GetNewColliderPtr(pObj, pObjPhyComponent);
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

	//ULONGLONG tickDiff = currentTick - _lastProcessedTick;
	//if (tickDiff < PHYSICS_TICK_RATE) {
	//	return;
	//}
	//_lastProcessedTick = currentTick;

	_pCollisionManager->RenewPhysicalComponents();
	_pCollisionManager->AdvanceFreeBodyMotion();
	_pCollisionManager->DetectCollision();

	// Handle Collision
	JStack* collisionPairs = _pCollisionManager->GetCollisionPairs();
	UINT32 size = collisionPairs->GetCount();
	CollisionPair* pair = (CollisionPair*)collisionPairs->First();
	for (UINT32 i = 0; i < size; ++i) {
		CalculateElasticCollisionNextMovements(pair->a, pair->b);
		ResolvePenetration(pair->a, pair->b);
		++pair;
	}

}

void StableFlow::CalculateElasticCollisionNextMovements(Collider* a, Collider* b)
{
	
	Vector3 n = a->_physicalComponent.transform.Position - b->_physicalComponent.transform.Position;
	if (Vector3::Norm(n) < EPSILON) {
		// Penetration
		return;
	}
	Vector3::Normalize(&n, n);

	Vector3 v = a->_physicalComponent.velocity * PHYSICS_TICK_RATE;
	Vector3 w = b->_physicalComponent.velocity * PHYSICS_TICK_RATE;

	Vector3 v_n = n * Vector3::DotProduct(n, v);
	Vector3 v_t = v - v_n;

	Vector3 w_n = n * Vector3::DotProduct(n, w);
	Vector3 w_t = w - w_n;

	Vector3 v_np = (v_n * (a->_physicalComponent.mass - b->_physicalComponent.mass) + w_n * (2.0f * b->_physicalComponent.mass)) * (1.0f / (a->_physicalComponent.mass + b->_physicalComponent.mass));
	Vector3 w_np = (w_n * (b->_physicalComponent.mass - a->_physicalComponent.mass) + v_n * (2.0f * a->_physicalComponent.mass)) * (1.0f / (a->_physicalComponent.mass + b->_physicalComponent.mass));

	a->_nextTransform.Position = a->_physicalComponent.transform.Position + v_n + v_t;
	b->_nextTransform.Position = b->_physicalComponent.transform.Position + w_np + w_t;
}

void StableFlow::ResolvePenetration(Collider* a, Collider* b)
{

}
