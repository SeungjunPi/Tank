#pragma once

#include "ICollisionManager.h"
#include "GameMath.h"

class CollisionManager : public ICollisionManager
{
public:
	CollisionManager();
	virtual ~CollisionManager() override;

	virtual Collider* GetNewColliderPtr(float radius, GameObject* pObj, const Vector3* center, UINT32 kindness);
	virtual Collider* GetAttachedColliderPtr(ColliderID id) override;
	virtual JStack* GetCollisionPairs() override;
	virtual void ReturnCollider(Collider* pColloder) override;

	// 이전 호출의 결과를 모두 초기화하고 현재 상태를 기준으로 검출함
	virtual void DetectCollision() override;

	void RenewPhysicalComponets();
private:
	Collider* _colliders = nullptr;
	ColliderID* _usedIDs = nullptr;
	UINT32 _countActiveColliders = 0;

	JStack _collisionPairs;

	void PopUsedIDs(ColliderID id);
	ColliderID GetUnusedID();
	void ResetAllColliderFlags();

	BOOL CheckCollision(Collider* one, Collider* another);
};




