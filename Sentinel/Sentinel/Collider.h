#pragma once


#include "stdafx.h"
#include "SentinelAliases.h"
#include "GameStruct.h"

const ColliderID INVALID_COLLIDER_ID = 0xffff;
const UINT16 MAX_SIMULTANEOUS_COLLISIONS = 3;

class CollisionManager;

// 현재 원형 충돌체만을 사용함. 
class Collider
{
	friend CollisionManager;
public:
	Collider();
	~Collider() = default;

	ColliderID GetID() const { return _id; }
	ObjectID GetObjectID() const { return _objectID; }
	Vector3 GetPosition() const { return _center; }

	void UpdateCenterPosition(const Vector3* position);

	// 최소한 MAX_SIMULTANEOUS_COLLISIONS 개수 이상의 공간이 out에 확보할 것을 권장. 
	UINT16 GetCollidingIDs(ColliderID* out);

	void ClearCollisionInfo();

	void Activate() { _isActive = TRUE; }
	void Deactivate() { _isActive = FALSE; }
	
private:
	Vector3 _center;
	float _radius;

	ColliderID _id;
	ObjectID _objectID;
	ColliderID _collidingIDs[MAX_SIMULTANEOUS_COLLISIONS];
	UINT16 _countColliding;

	BOOL _isActive = FALSE;

	void AddCollidingID(ColliderID otherColliderID);

	void Initiate(float radius, ObjectID objectID);

	// 반환하기 위해 내부를 모두 초기화하는 용도로 사용
	void Clear();

	BOOL IsAttatchedToGameObject() const { return _objectID != INVALID_OBJECT_ID; }
};
