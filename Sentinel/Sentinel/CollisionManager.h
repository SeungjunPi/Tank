#pragma once


#include "Collider.h"

const int MAX_NUM_COLLIDERS = 2048;

// 자체 메모리풀 사용
class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();

	Collider* GetNewColliderPtr(int radius, ObjectID objectID);
	void ReturnCollider(Collider* pColloder);

	UINT32 DetectCollision(ColliderID** out);
	void ResetAllColliders();
private:
	Collider* _colliders = nullptr;
	ColliderID* _usedIDs = nullptr;
	ColliderID* _collidedColliderIDs = nullptr;
	UINT32 _countActiveColliders = 0;

	void PopUsedIDs(ColliderID id);
	ColliderID GetUnusedID();
};

