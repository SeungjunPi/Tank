#include "CollisionManager.h"
#include "Collider.h"

CollisionManager::CollisionManager()
{
	_colliders = DNew Collider[MAX_NUM_COLLIDERS];
	for (UINT16 i = 0; i < MAX_NUM_COLLIDERS; ++i) {
		_colliders[i]._id = (ColliderID)i;
	}
	_usedIDs = DNew ColliderID[MAX_NUM_COLLIDERS];
	_collidedColliderIDs = DNew ColliderID[MAX_NUM_COLLIDERS];
}

CollisionManager::~CollisionManager()
{
	delete[] _colliders;
	delete[] _usedIDs;
	delete[] _collidedColliderIDs;
}

Collider* CollisionManager::GetNewColliderPtr(float radius, GameObject* pObj)
{
	ColliderID id = GetUnusedID();
	Collider* pCollider = _colliders + id;
	pCollider->Initiate(radius, pObj);
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

void CollisionManager::ReturnCollider(Collider* pCollider)
{
	ColliderID key = pCollider->GetID();
	pCollider->Clear();
	PopUsedIDs(key);
}

UINT32 CollisionManager::DetectCollision(ColliderID** out)
{
	ResetAllColliders();

	UINT32 countCollidedColliders = 0;
	*out = _collidedColliderIDs;
	for (size_t i = 0; i < _countActiveColliders; ++i) {
		ColliderID firstID = _usedIDs[i];
		bool isFirstColliderColide = false;
		Collider* pCollider1 = _colliders + firstID;

		if (!pCollider1->_isActive) {
			continue;
		}

		for (size_t j = 0; j < _countActiveColliders; ++j) {
			if (i == j) {
				continue;
			}
			ColliderID secondID = _usedIDs[j];

			Collider* pCollider2 = _colliders + secondID;
			if (!pCollider2->_isActive) {
				continue;
			}
			
			float firstColliderRadius = pCollider1->_radius;
			float secondColliderRadius = pCollider2->_radius;
			float thresholdSq = (firstColliderRadius + secondColliderRadius) * (firstColliderRadius + secondColliderRadius);

			if (Vector3::DistanceSquared(pCollider1->GetPosition(), pCollider2->GetPosition()) < thresholdSq) {
				pCollider1->AddCollidingID(secondID);
				isFirstColliderColide = true;
			}
		}

		if (isFirstColliderColide) {
			_collidedColliderIDs[countCollidedColliders] = firstID;
			++countCollidedColliders;
		}
	}

	return countCollidedColliders;
}

void CollisionManager::ResetAllColliders()
{
	for (UINT32 i = 0; i < _countActiveColliders; ++i) {
		ColliderID id = _usedIDs[i];
		_colliders[id].ClearCollisionInfo();
	}
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
