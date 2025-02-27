#pragma once

#include "IStableFlow.h"

class CollisionManager;


class StableFlow : public IStableFlow
{
public:
	StableFlow();
	virtual ~StableFlow();

	virtual Collider* GetNewColliderPtr(GameObject* pObj, PhysicalComponent* pObjPhyComponent) override;
	virtual Collider* GetAttachedColliderPtr(ColliderID id) override;
	virtual void ReturnCollider(Collider* pColloder) override;

	virtual JStack* GetCollisionPairs() override;
	

	
	virtual void ProcessStableFlow(ULONGLONG currentTick) override;

private:
	ULONGLONG _lastProcessedTick = 0;
	CollisionManager* _pCollisionManager;

	// 현재 Z=0 평면에서만 처리함.
	void CalculateElasticCollisionNextMovements(Collider* a, Collider* b);

	// 현재 Z=0 평면에서만 처리함.
	void ResolvePenetration(Collider* a, Collider* b);
};



