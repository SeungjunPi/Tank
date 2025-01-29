#define _USE_MATH_DEFINES

#include "CollisionManager.h"
#include "Collider.h"

#include <time.h>
#include <cmath>

CollisionManager::CollisionManager()
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

Collider* CollisionManager::GetNewColliderPtr(float radius, GameObject* pObj, const Vector3* center, const Vector3* velocity, float mass, UINT32 kindness)
{
	ColliderID id = GetUnusedID();
	Collider* pCollider = _colliders + id;
	pCollider->Initiate(radius, pObj, center, velocity, mass, kindness);
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

void CollisionManager::DetectCollision()
{
	ResetAllColliders();

	for (size_t i = 0; i < _countActiveColliders; ++i) {
		for (size_t j = i + 1; j < _countActiveColliders; ++j) {
			ColliderID oneID = _usedIDs[i];
			Collider* pColliderOne = _colliders + oneID;
			ColliderID anotherID = _usedIDs[j];
			Collider* pColliderAnother = _colliders + anotherID;

			if (!CheckCollision(pColliderOne, pColliderAnother)) {
				continue;
			}

			pColliderOne->_collisionKindnessFlag |= pColliderAnother->_kindness;
			pColliderAnother->_collisionKindnessFlag |= pColliderOne->_kindness;
			CalculateElasticCollisionNextMovements(pColliderOne, pColliderAnother);
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

void CollisionManager::CalculateElasticCollisionNextMovements(Collider* a, Collider* b)
{
	Vector3 n = a->_center - b->_center;
	const float samePositionThreashold = 1E-05;
	if (Vector3::Norm(n) < samePositionThreashold) {
		a->_collisionKindnessFlag |= 0x01 << 31; // kindness 확장시 변경 필요
		b->_collisionKindnessFlag |= 0x01 << 31; // kindness 확장시 변경 필요

		// 임시적으로 평면상의 위치를 밀어냄. 
		float rad = (float)rand() / RAND_MAX * 2.0f * M_PI;
		Vector3 randomVector = { cosf(rad), sinf(rad), 0 };

		float aWeight = (a->_radius + b->_radius) * b->_mass / (a->_mass + b->_mass);
		float bWeight = (a->_radius + b->_radius) * a->_mass / (a->_mass + b->_mass);

		a->_nextMovement = randomVector * aWeight;
		b->_nextMovement = randomVector * -bWeight;

		return;
	}
	
	Vector3::Normalize(&n, n);

	Vector3 v = a->_velocity;
	Vector3 w = b->_velocity;

	Vector3 v_n = n * Vector3::DotProduct(n, v);
	Vector3 v_t = v - v_n;

	Vector3 w_n = n * Vector3::DotProduct(n, w);
	Vector3 w_t = w - w_n;

	Vector3 v_np = (v_n * (a->_mass - b->_mass) + w_n * (2.0f * b->_mass)) * (1.0f / (a->_mass + b->_mass));
	Vector3 w_np = (w_n * (b->_mass - a->_mass) + v_n * (2.0f * a->_mass)) * (1.0f / (a->_mass + b->_mass));
	
	a->_nextMovement = v_np + v_t;
	b->_nextMovement = w_np + w_t;
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

