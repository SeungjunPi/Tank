#pragma once

#include "CollisionPch.h"
#include "CollisionAlias.h"
#include "GameMath.h"
#include "JStack.h"
#include "StableFlowStaticData.h"

class Collider;
class GameObject;



class IStableFlow
{
public:
	virtual ~IStableFlow() {}

	// Collider 관련 
	// 사용 종료 시 반드시 ReturnCollider를 통해 반환해야 함.
	virtual Collider* GetNewColliderPtr(GameObject* pObj, PhysicalComponent* pObjPhyComponent) = 0;
	virtual Collider* GetAttachedColliderPtr(ColliderID id) = 0;
	virtual JStack* GetCollisionPairs() = 0; // Deprecated
	virtual void ReturnCollider(Collider* pColloder) = 0;

	// 물리 계산 시작
	virtual void ProcessStableFlow(ULONGLONG currentTick) = 0;
};

void CreateStableFlow(IStableFlow** dst);
void DeleteStableFlow(IStableFlow* ptr);
