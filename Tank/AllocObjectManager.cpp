#include "AllocObjectManager.h"
#include "GameObject.h"
#include "Global.h"
#include "ICollisionManager.h"

static const int MAX_NUM_OBJECT = 4096;

void AllocObjectManager::Initiate()
{
	_tankTable.Initiate(MAX_NUM_OBJECT);
	_projectileTable.Initiate(MAX_NUM_OBJECT);
	_obstacleTable.Initiate(MAX_NUM_OBJECT);
}

void AllocObjectManager::Terminate()
{
	UINT32* keys = DNew UINT32[MAX_NUM_OBJECT];
	UINT32 numCounts = 0;

	_tankTable.GetIdsTo(keys, &numCounts);
	for (int i = 0; i < numCounts; ++i) {
		UINT32 key = keys[i];
		void* ptr = _tankTable.Pop(key);
		delete ptr;
	}

	_projectileTable.GetIdsTo(keys, &numCounts);
	for (int i = 0; i < numCounts; ++i) {
		UINT32 key = keys[i];
		void* ptr = _projectileTable.Pop(key);
		delete ptr;
	}

	_obstacleTable.GetIdsTo(keys, &numCounts);
	for (int i = 0; i < numCounts; ++i) {
		UINT32 key = keys[i];
		void* ptr = _obstacleTable.Pop(key);
		delete ptr;
	}

	delete[] keys;

	_projectileTable.Terminate();
	_obstacleTable.Terminate();
	_tankTable.Terminate();
}

Tank* AllocObjectManager::CreateTank(ObjectID objectID, UserDBIndex ownerIndex)
{
	Tank* pTank = DNew Tank(objectID, ownerIndex);

	Collider* pCollider = g_pCollisionManager->GetNewColliderPtr(Tank::COLLIDER_RADIUS, pTank);
	pTank->AttachCollider(pCollider);

	bool res = _tankTable.Insert(objectID.key, pTank);
	if (!res) {
		__debugbreak();
	}
	return pTank;
}

Projectile* AllocObjectManager::CreateProjectile(ObjectID objectID, Transform* pInitTransform, UserDBIndex ownerIndex)
{
	Projectile* pProjectile = DNew Projectile();
	Collider* pCollider = g_pCollisionManager->GetNewColliderPtr(Projectile::COLLIDER_RADIUS, pProjectile);
	pProjectile->AttachCollider(pCollider);
	pProjectile->Initiate(objectID, pInitTransform, ownerIndex);
	_projectileTable.Insert(objectID.key, pProjectile);
	return pProjectile;
}

Obstacle* AllocObjectManager::CreateObstacle(ObjectID objectID, Transform* pInitTransform, UserDBIndex ownerIndex)
{
	/*
	Obstacle* pObstacle = DNew Obstacle();
	pObstacle->Initiate(objectID, pInitTransform);
	_obstacleTable.Insert(objectID.key, pObstacle);
	return pObstacle;*/

	return nullptr;
}

void AllocObjectManager::RemoveObject(EGameObjectType objectKind, ObjectKey objectKey)
{
	void* ptr = nullptr;
	switch (objectKind) {
	case GAME_OBJECT_TYPE_TANK:
	{
		ptr = _tankTable.Pop(objectKey);
		Tank* pTank = (Tank*)ptr;
		Collider* pCollider = pTank->GetColliderPtr();
		g_pCollisionManager->ReturnCollider(pCollider);
		break;
	}
	case GAME_OBJECT_TYPE_PROJECTILE:
	{
		ptr = _projectileTable.Pop(objectKey);
		Projectile* pProjectile = (Projectile*)ptr;
		Collider* pCollider = pProjectile->GetColliderPtr();
		g_pCollisionManager->ReturnCollider(pCollider);
		break;
	}
	case GAME_OBJECT_TYPE_OBSTACLE:
	{
		ptr = _obstacleTable.Pop(objectKey);
		break;
	}
	}

	if (ptr != nullptr) {
		delete ptr;
	}
}

void AllocObjectManager::RemoveObject(ObjectID objectID)
{
	RemoveObject(objectID.type, objectID.key);
}

void AllocObjectManager::UpdateObject(GameObject* obj)
{
	if (obj == nullptr) {
		__debugbreak();
		return;
	}

	ObjectID objectID = obj->GetID();
	GameObject* pGameObject = GetObjectPtrOrNull(objectID);

	BOOL res = pGameObject->UpdateFrom(obj);
	assert(res);
}

void AllocObjectManager::UpdateObjectTransform(ObjectID objId, const Transform* pTransform)
{
	GameObject* pGameObject = GetObjectPtrOrNull(objId);
	if (pGameObject == nullptr) {
		return;
	}
	pGameObject->UpdateTransform(pTransform);
}

void AllocObjectManager::GetTransformedModelOf(EGameObjectType objectKind, ObjectKey key, Vertex* out_vertices, UINT* out_numVectors)
{
	GameObject* pGameObject = GetObjectPtrOrNull(objectKind, key);
	*out_numVectors = pGameObject->GetTransformedModel(out_vertices);
}

void AllocObjectManager::GetTransformedModelOf(ObjectID objectID, Vertex* out_vertices, UINT* out_numVectors)
{
	GetTransformedModelOf(objectID.type, objectID.key, out_vertices, out_numVectors);
}

void AllocObjectManager::GetKeys(EGameObjectType objectKind, UINT32* out_keys, UINT32* out_numKeys) const
{
	if (out_keys == nullptr || out_numKeys == nullptr) {
		__debugbreak();
		return;
	}

	UINT32* keys = out_keys;

	switch (objectKind) {
		break;
	case GAME_OBJECT_TYPE_TANK:
		_tankTable.GetIdsTo(keys, out_numKeys);
		break;
	case GAME_OBJECT_TYPE_PROJECTILE:
		_projectileTable.GetIdsTo(keys, out_numKeys);
		break;
	case GAME_OBJECT_TYPE_OBSTACLE:
		_obstacleTable.GetIdsTo(keys, out_numKeys);
		break;
	default:
		__debugbreak();
		break;
	}
}

void AllocObjectManager::GetAllKeys(UINT32* out_keys, UINT32* out_numKeys) const
{
	if (out_keys == nullptr || out_numKeys == nullptr) {
		__debugbreak();
		return;
	}

	UINT32* keys = out_keys;
	UINT32 countKeys = 0;

	_tankTable.GetIdsTo(keys, &countKeys);
	*out_numKeys += countKeys;

	keys += countKeys;
	_projectileTable.GetIdsTo(keys, &countKeys);
	*out_numKeys += countKeys;

	keys += countKeys;
	_obstacleTable.GetIdsTo(keys, &countKeys);
	*out_numKeys += countKeys;

}

GameObject* AllocObjectManager::GetObjectPtrOrNull(EGameObjectType objectKind, ObjectKey objectKey)
{
	GameObject* pGameObject = nullptr;

	switch (objectKind) {
	case GAME_OBJECT_TYPE_TANK:
		pGameObject = (GameObject*)_tankTable.Get(objectKey);
		break;
	case GAME_OBJECT_TYPE_PROJECTILE:
		pGameObject = (GameObject*)_projectileTable.Get(objectKey);
		break;
	case GAME_OBJECT_TYPE_OBSTACLE:
		pGameObject = (GameObject*)_obstacleTable.Get(objectKey);
		break;
	default:
		pGameObject = nullptr;
		__debugbreak();
		break;
	}

	return pGameObject;
}

GameObject* AllocObjectManager::GetObjectPtrOrNull(ObjectID objectID)
{
	return GetObjectPtrOrNull(objectID.type, objectID.key);
}

int AllocObjectManager::GetCountObjects() const
{
	int countObject = 0;
	countObject += _tankTable.GetCount();
	countObject += _projectileTable.GetCount();
	countObject += _obstacleTable.GetCount();
	return countObject;
}

void AllocObjectManager::StartTankMove(ObjectID objectId, EMOVEMENT movement)
{
	Tank* pTank = (Tank*)_tankTable.Get(objectId.key);
	if (pTank == nullptr) {
		return;
	}

	pTank->StartMove(movement);
}

void AllocObjectManager::EndTankMove(ObjectID objectId, EMOVEMENT movement, const Transform* pTransform)
{
	Tank* pTank = (Tank*)_tankTable.Get(objectId.key);
	if (pTank == nullptr) {
		return;
	}

	pTank->EndMove(movement, pTransform);
}

void AllocObjectManager::StartTankRotate(ObjectID objectId, EROTATION rotation)
{
	Tank* pTank = (Tank*)_tankTable.Get(objectId.key);
	if (pTank == nullptr) {
		return;
	}

	pTank->StartRotate(rotation);
}

void AllocObjectManager::EndTankRotate(ObjectID objectId, EROTATION rotation, const Transform* pTransform)
{
	Tank* pTank = (Tank*)_tankTable.Get(objectId.key);
	if (pTank == nullptr) {
		return;
	}

	pTank->EndRotate(rotation, pTransform);
}
