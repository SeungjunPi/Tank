#pragma once

#include "CollisionPch.h"
#include "CollisionAlias.h"
#include "GameMath.h"
#include "JStack.h"


const int MAX_NUM_COLLIDERS = 2048;

class Collider;
class GameObject;

struct PhysicalComponent;

struct CollisionPair
{
	Collider* a = nullptr;
	Collider* b = nullptr;
};

// 자체 메모리풀 사용
// 총 충돌 수가 충돌체 * 10 만큼 검출 가능.
class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();

	Collider* GetNewColliderPtr(GameObject* pObj, PhysicalComponent* objectPhysicalComponent);
	Collider* GetAttachedColliderPtr(ColliderID id);
	JStack* GetCollisionPairs();
	void ReturnCollider(Collider* pColloder);

	// 이전 호출의 결과를 모두 초기화하고 현재 상태를 기준으로 검출함
	void DetectCollision();

	void AdvanceFreeBodyMotion();

	void RenewPhysicalComponents();
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




