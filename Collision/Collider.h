#pragma once

#include "CollisionPch.h"
#include "CollisionAlias.h"
#include "GameMath.h"
#include "StableFlowStaticData.h"

class CollisionManager;
class GameObject;

// 현재 원형 충돌체만을 사용함. 
class Collider
{
	friend CollisionManager;
public:
	Collider();
	~Collider() = default;

 	ColliderID GetID() const { return _id; }
	Vector3 GetCenter() const { return _physicalComponent.transform.Position; }

	GameObject* GetAttachedObjectPtr() const { return _pObject; }

	BOOL IsCollided() const { return _collisionKindnessFlag != 0; }
	UINT32 GetCollisionKindnessFlag() const { return _collisionKindnessFlag; }
	
	void ResetCollisionFlag();

	BOOL IsActive() const { return _isActive; }
	void Activate() { _isActive = TRUE; }
	void Deactivate() { _isActive = FALSE; }

	void UpdatePhysicsComponentFromGameObject();
	void OverwriteComputedResultsToGameObject();
	void GetComputedResults(Transform* out_transform, Vector3* out_velocity, Vector3* out_angularVelocity) const;
	
private:
	ColliderID _id = INVALID_COLLIDER_ID;

	PhysicalComponent _physicalComponent;

	Transform _nextTransform;
	Vector3 _nextVelocity;
	Vector3 _nextAngularVelocity;

	UINT32 _collisionKindnessFlag = 0;
	
	GameObject* _pObject = nullptr;
	PhysicalComponent* _pObjectPhysicalComponent = nullptr;
	BOOL _isActive = FALSE;

	void Initiate(GameObject* pObj, PhysicalComponent* objectPhysicalComponent);

	// 반환하기 위해 내부를 모두 초기화하는 용도로 사용
	void Clear();

	BOOL IsAttatchedToGameObject() const { return _pObject != nullptr; }
};

