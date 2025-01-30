#pragma once

#include "CollisionPch.h"
#include "CollisionAlias.h"
#include "GameMath.h"

const ColliderID INVALID_COLLIDER_ID = 0xffff;

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
	Vector3 GetCenter() const { return _center; }

	void UpdateCenter(const Vector3& position);

	GameObject* GetAttachedObjectPtr() const { return _pObject; }

	BOOL IsCollided() const { return _collisionKindnessFlag != 0; }
	UINT32 GetCollisionKindnessFlag() const { return _collisionKindnessFlag; }
	
	void ResetCollisionFlag();

	BOOL IsActive() { return _isActive; }
	void Activate() { _isActive = TRUE; }
	void Deactivate() { _isActive = FALSE; }
	
private:
	ColliderID _id = INVALID_COLLIDER_ID;

	// Decide Collider
	Vector3 _center;
	float _radius = 0.0f;

	UINT32 _kindness = 0;
	UINT32 _collisionKindnessFlag = 0;
	
	GameObject* _pObject = nullptr;
	BOOL _isActive = FALSE;

	void Initiate(float radius, GameObject* pObj, const Vector3* center, UINT32 kindness);

	// 반환하기 위해 내부를 모두 초기화하는 용도로 사용
	void Clear();

	BOOL IsAttatchedToGameObject() const { return _pObject != nullptr; }
};

