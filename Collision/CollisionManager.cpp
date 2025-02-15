#define _USE_MATH_DEFINES

#include "CollisionManager.h"
#include "Collider.h"

#include <time.h>
#include <cmath>

const float SAME_POSITION_THRESHOLD = 1E-04f;


CollisionManager::CollisionManager()
	: _collisionPairs(sizeof(CollisionPair), MAX_NUM_COLLIDERS * 10)
{
	_colliders = DNew Collider[MAX_NUM_COLLIDERS];
	for (UINT16 i = 0; i < MAX_NUM_COLLIDERS; ++i) {
		_colliders[i]._id = (ColliderID)i;
	}
	_usedIDs = DNew ColliderID[MAX_NUM_COLLIDERS];

	srand((unsigned)(time(0)));
}

CollisionManager::~CollisionManager()
{
	delete[] _colliders;
	delete[] _usedIDs;
}

Collider* CollisionManager::GetNewColliderPtr(float radius, GameObject* pObj, const Vector3* center, UINT32 kindness)
{
	ColliderID id = GetUnusedID();
	Collider* pCollider = _colliders + id;
	pCollider->Initiate(radius, pObj, center, kindness);
	_usedIDs[_countActiveColliders] = id;
	++_countActiveColliders;
	
	return pCollider;
}

Collider* CollisionManager::GetAttachedColliderPtr(ColliderID id)
{
	Collider* pCol = _colliders + id;
	if (pCol->IsAttatchedToGameObject()) {
		return pCol;
	}
	
	return nullptr;
}

JStack* CollisionManager::GetCollisionPairs()
{
	return &_collisionPairs;
}

void CollisionManager::ReturnCollider(Collider* pCollider)
{
	ColliderID key = pCollider->GetID();
	pCollider->Clear();
	PopUsedIDs(key);
}

void CollisionManager::DetectCollision()
{
	ResetAllColliders();
	_collisionPairs.Clear();

	for (size_t i = 0; i < _countActiveColliders; ++i) {
		for (size_t j = i + 1; j < _countActiveColliders; ++j) {
			ColliderID oneID = _usedIDs[i];
			Collider* pColliderOne = _colliders + oneID;
			ColliderID anotherID = _usedIDs[j];
			Collider* pColliderAnother = _colliders + anotherID;

			if (CheckCollision(pColliderOne, pColliderAnother)) {
				CollisionPair pair{ pColliderOne->_pObject, pColliderAnother->_pObject };
				_collisionPairs.Push(&pair);
				pColliderOne->_collisionKindnessFlag = 0x01;
				pColliderAnother->_collisionKindnessFlag = 0x01;
			}
		}
	}
}

void CollisionManager::ResetAllColliders()
{
	for (UINT32 i = 0; i < _countActiveColliders; ++i) {
		ColliderID id = _usedIDs[i];
		_colliders[id].ResetCollisionFlag();
	}
}

BOOL CollisionManager::CheckCollision(Collider* one, Collider* another)
{
	if (!one->_isActive || !another->_isActive) {
		return FALSE;
	}

	float distance = Vector3::DistanceSquared(one->_center, another->_center);
	float radiusSum = one->_radius + another->_radius;
	if (distance < radiusSum * radiusSum) {
		return TRUE;
	}
	return FALSE;
}

void CollisionManager::PopUsedIDs(ColliderID id)
{
	for (size_t i = 0; i < MAX_NUM_COLLIDERS; ++i) {
		if (id == _usedIDs[i]) {
			--_countActiveColliders;
			_usedIDs[i] = _usedIDs[_countActiveColliders];
			break;
		}
	}
}

// collider를 0부터 순회하며 사용중이지 않은 collider의 ID를 전달함. 따라서 기본적으로 가장 작은 ID를 전달함. 
ColliderID CollisionManager::GetUnusedID()
{
	ColliderID unusedID = INVALID_COLLIDER_ID;
	for (ColliderID i = 0; i < MAX_NUM_COLLIDERS; ++i) {
		if (_colliders[i]._radius == 0) {
			unusedID = i;
			break;
		}
	}

	if (unusedID == INVALID_COLLIDER_ID) {
		__debugbreak();
	}

	return unusedID;
}

