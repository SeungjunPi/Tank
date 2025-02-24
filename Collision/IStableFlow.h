#pragma once

#include "CollisionPch.h"
#include "CollisionAlias.h"
#include "GameMath.h"
#include "JStack.h"

class Collider;
class GameObject;

class IStableFlow
{
public:

	// Collider 관련 
	// 사용 종료 시 반드시 ReturnCollider를 통해 반환해야 함.
	virtual Collider* GetNewColliderPtr(float radius, GameObject* pObj, const Vector3* center, UINT32 kindness) = 0;
	virtual Collider* GetAttachedColliderPtr(ColliderID id) = 0;
	virtual JStack* GetCollisionPairs() = 0;
	virtual void ReturnCollider(Collider* pColloder) = 0;

	// 물리 계산 시작
	virtual void ProcessStableFlow() = 0;
};

void CreateStableFlow(IStableFlow** dst);
void DeleteStableFlow(IStableFlow* ptr);
