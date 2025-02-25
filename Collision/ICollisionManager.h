#pragma once

#include "CollisionPch.h"
#include "CollisionAlias.h"
#include "GameMath.h"
#include "JStack.h"

// Deprecated
class ICollisionManager
{
public:
	virtual ~ICollisionManager() = default;

	virtual Collider* GetNewColliderPtr(float radius, GameObject* pObj, const Vector3* center, UINT32 kindness) = 0;
	virtual Collider* GetAttachedColliderPtr(ColliderID id) = 0;
	virtual JStack* GetCollisionPairs() = 0;
	virtual void ReturnCollider(Collider* pColloder) = 0;

	// 이전 호출의 결과를 모두 초기화하고 현재 상태를 기준으로 검출함
	virtual void DetectCollision() = 0;
};

void CreateCollisionManager(ICollisionManager** dst);
void DeleteCollisionManager(ICollisionManager* ptr);


