#pragma once

#include "CollisionPch.h"
#include "CollisionAlias.h"
#include "GameMath.h"

const int MAX_NUM_COLLIDERS = 2048;

class Collider;
class GameObject;

// 자체 메모리풀 사용
// 하나의 충돌체가 한 번에 충돌체 세 개를 초과하여 충돌하는 경우는 오류 상황으로 간주.
class ICollisionManager
{
public:
	virtual ~ICollisionManager() = default;

	// Kindess는 Flag로써 다른 Collider와 충돌 시 해당 Collider의 Kindness를 BitMasking으로써 저장한다. Kindness를 Bit로 사용하지 않으면 구분할 수 없게 될 수 없으므로 주의.
	virtual Collider* GetNewColliderPtr(float radius, GameObject* pObj, const Vector3* center, const Vector3* velocity, float mass, UINT32 kindness) = 0;
	virtual Collider* GetAttachedColliderPtr(ColliderID id) = 0;
	virtual void ReturnCollider(Collider* pColloder) = 0;

	// 이전 호출의 결과를 모두 초기화하고 현재 상태를 기준으로 검출함
	virtual void DetectCollision() = 0;
};

void CreateCollisionManager(ICollisionManager** dst);
void DeleteCollisionManager(ICollisionManager* ptr);


