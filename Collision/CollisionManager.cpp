#define _USE_MATH_DEFINES

#include "CollisionManager.h"
#include "Collider.h"
#include "StableFlowStaticData.h"

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

Collider* CollisionManager::GetNewColliderPtr(GameObject* pObj, PhysicalComponent* objectPhysicalComponent)
{
	ColliderID id = GetUnusedID();
	Collider* pCollider = _colliders + id;
	pCollider->Initiate(pObj, objectPhysicalComponent);
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
	ResetAllColliderFlags();
	_collisionPairs.Clear();

	for (size_t i = 0; i < _countActiveColliders; ++i) {
		for (size_t j = i + 1; j < _countActiveColliders; ++j) {
			ColliderID oneID = _usedIDs[i];
			Collider* pColliderOne = _colliders + oneID;
			ColliderID anotherID = _usedIDs[j];
			Collider* pColliderAnother = _colliders + anotherID;

			if (CheckCollision(pColliderOne, pColliderAnother)) {
				CollisionPair pair{ pColliderOne, pColliderAnother };
				_collisionPairs.Push(&pair);
				pColliderOne->_collisionKindnessFlag = 0x01;
				pColliderAnother->_collisionKindnessFlag = 0x01;
			}
		}
	}
}

void CollisionManager::AdvanceFreeBodyMotion()
{
	for (size_t i = 0; i < _countActiveColliders; ++i) {
		ColliderID id = _usedIDs[i];
		Collider* pCollider = _colliders + id;

		if (pCollider->IsCollided()) {
			continue;
		}

		
		// FreeBodyMotion
		pCollider->_nextTransform.Position = pCollider->_physicalComponent.transform.Position + pCollider->_physicalComponent.velocity * PHYSICS_TICK_RATE;
		// Rotation
		Quaternion angularVelocityQ;
		// angularVelocityQ = Quaternion::ConvertFromVector3(pCollider->_physicalComponent.angularVelocity * PHYSICS_TICK_RATE); // 이거 구현 안하면 탱크 안돌아감..
		pCollider->_nextTransform.Rotation = Quaternion::Product(pCollider->_physicalComponent.transform.Rotation, angularVelocityQ);

		pCollider->_nextVelocity = pCollider->_physicalComponent.velocity;
		pCollider->_nextAngularVelocity = pCollider->_physicalComponent.angularVelocity;
	}
}

void CollisionManager::RenewPhysicalComponents()
{
	for (size_t i = 0; i < _countActiveColliders; ++i) {
		ColliderID id = _usedIDs[i];
		Collider* pCollider = _colliders + id;
		pCollider->UpdatePhysicsComponentFromGameObject();
	}
}

void CollisionManager::ResetAllColliderFlags()
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

	float distanceSq = Vector3::DistanceSquared(one->_physicalComponent.transform.Position, another->_physicalComponent.transform.Position);
	float radiusSum = one->_physicalComponent.radius + another->_physicalComponent.radius;
	if (distanceSq < radiusSum * radiusSum) {
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
		if (_colliders[i]._physicalComponent.radius == 0) {
			unusedID = i;
			break;
		}
	}

	if (unusedID == INVALID_COLLIDER_ID) {
		__debugbreak();
	}

	return unusedID;
}

