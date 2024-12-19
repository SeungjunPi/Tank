#pragma once


#include "Collider.h"

const int MAX_NUM_COLLIDERS = 2048;

// 자체 메모리풀 사용
class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();

	Collider* GetNewColliderPtr(float radius, ObjectID objectID);
	Collider* GetActiveColliderPtr(ColliderID id);
	void ReturnCollider(Collider* pColloder);

	// 이전 호출의 결과를 모두 초기화하고 현재 상태를 기준으로 검출함
	UINT32 DetectCollision(ColliderID** out);
private:
	Collider* _colliders = nullptr;
	ColliderID* _usedIDs = nullptr;
	ColliderID* _collidedColliderIDs = nullptr;
	UINT32 _countActiveColliders = 0;

	void PopUsedIDs(ColliderID id);
	ColliderID GetUnusedID();
	void ResetAllColliders();
};

