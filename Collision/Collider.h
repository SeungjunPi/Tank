#pragma once

#include "CollisionPch.h"
#include "CollisionAlias.h"
#include "GameMath.h"

const ColliderID INVALID_COLLIDER_ID = 0xffff;
const UINT16 MAX_SIMULTANEOUS_COLLISIONS = 3;

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
	GameObject* GetAttachedObjectPtr() const { return _pObject; }
	Vector3 GetPosition() const { return _center; }

	void UpdateCenterPosition(const Vector3* position);

	// 최소한 MAX_SIMULTANEOUS_COLLISIONS 개수 이상의 공간을 out에 확보할 것을 권장. 
	UINT16 GetCollidingIDs(ColliderID* out);

	void ClearCollisionInfo();

	void Activate() { _isActive = TRUE; }
	void Deactivate() { _isActive = FALSE; }
	
private:
	Vector3 _center;
	float _radius = 0.0f;

	ColliderID _id = INVALID_COLLIDER_ID;
	GameObject* _pObject = nullptr;
	ColliderID _collidingIDs[MAX_SIMULTANEOUS_COLLISIONS];
	UINT16 _countColliding = 0;

	BOOL _isActive = FALSE;

	void AddCollidingID(ColliderID otherColliderID);

	void Initiate(float radius, GameObject* pObj);

	// 반환하기 위해 내부를 모두 초기화하는 용도로 사용
	void Clear();

	BOOL IsAttatchedToGameObject() const { return _pObject != nullptr; }
};

