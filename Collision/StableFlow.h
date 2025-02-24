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
	

	
	virtual void ProcessStableFlow() override;

private:
	CollisionManager* _pCollisionManager;

	void CalculateElasticCollisionNextMovements(GameObject* a, GameObject* b, ULONGLONG tickDiff);
	void ResolvePenetration(GameObject* a, GameObject* b);
};



