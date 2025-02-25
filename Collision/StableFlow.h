#pragma once

#include "IStableFlow.h"

class CollisionManager;


class StableFlow : public IStableFlow
{
public:
	StableFlow();
	virtual ~StableFlow();

	virtual Collider* GetNewColliderPtr(float radius, GameObject* pObj, const Vector3* center, UINT32 kindness) override;
	virtual Collider* GetAttachedColliderPtr(ColliderID id) override;
	virtual void ReturnCollider(Collider* pColloder) override;

	virtual JStack* GetCollisionPairs() override;
	

	
	virtual void ProcessStableFlow(ULONGLONG currentTick) override;

private:
	ULONGLONG _lastProcessedTick = 0;
	CollisionManager* _pCollisionManager;

	void CalculateElasticCollisionNextMovements(Collider* a, Collider *b, ULONGLONG tickDiff);
	void ResolvePenetration(Collider* a, Collider* b);
};



