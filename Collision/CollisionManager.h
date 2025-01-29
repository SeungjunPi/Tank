#pragma once

#include "ICollisionManager.h"
#include "GameMath.h"

// 자체 메모리풀 사용
// 하나의 충돌체가 한 번에 충돌체 세 개를 초과하여 충돌하는 경우는 오류 상황으로 간주.
class CollisionManager : public ICollisionManager
{
public:
	CollisionManager();
	virtual ~CollisionManager() override;

	virtual Collider* GetNewColliderPtr(float radius, GameObject* pObj, const Vector3* center, float mass, UINT32 kindness) override;
	virtual Collider* GetAttachedColliderPtr(ColliderID id) override;
	virtual void ReturnCollider(Collider* pColloder) override;

	// 이전 호출의 결과를 모두 초기화하고 현재 상태를 기준으로 검출함
	virtual void DetectCollision() override;
private:
	Collider* _colliders = nullptr;
	ColliderID* _usedIDs = nullptr;
	UINT32 _countActiveColliders = 0;

	void PopUsedIDs(ColliderID id);
	ColliderID GetUnusedID();
	void ResetAllColliders();


	BOOL CheckCollision(Collider* one, Collider* another);
	void CalculateElasticCollisionNextMovements(Collider* one, Collider* another);
};




