#pragma once

#include "ICollisionManager.h"
#include "GameMath.h"

// 자체 메모리풀 사용
// 총 충돌 수가 충돌체 * 10 만큼 검출 가능.
class CollisionManager : public ICollisionManager
{
public:
	CollisionManager();
	virtual ~CollisionManager() override;

	virtual Collider* GetNewColliderPtr(float radius, GameObject* pObj, const Vector3* center, const Vector3* direction, float mass, UINT32 kindness) override;
	virtual Collider* GetAttachedColliderPtr(ColliderID id) override;
	virtual JStack* GetCollisionPairs() override;
	virtual void ReturnCollider(Collider* pColloder) override;

	// 이전 호출의 결과를 모두 초기화하고 현재 상태를 기준으로 검출함
	virtual void DetectCollision() override;
private:
	Collider* _colliders = nullptr;
	ColliderID* _usedIDs = nullptr;
	UINT32 _countActiveColliders = 0;

	JStack _collisionPairs;

	void PopUsedIDs(ColliderID id);
	ColliderID GetUnusedID();
	void ResetAllColliders();


	BOOL CheckCollision(Collider* one, Collider* another);
	void CalculateElasticCollisionNextMovements(Collider* one, Collider* another);
	void ResolvePenetration(Collider* one, Collider* another);
};




