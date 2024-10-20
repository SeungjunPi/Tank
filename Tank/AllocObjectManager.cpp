#include "AllocObjectManager.h"
#include "GameObject.h"

static const int MAX_NUM_OBJECT = 4096;

void AllocObjectManager::Initiate()
{
	_tankTable.Initiate(MAX_NUM_OBJECT);
	_projectileTable.Initiate(MAX_NUM_OBJECT);
	_obstacleTable.Initiate(MAX_NUM_OBJECT);
}

void AllocObjectManager::Terminate()
{
	int* keys = new int[MAX_NUM_OBJECT];
	int numCounts = 0;

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

Tank* AllocObjectManager::CreateTank(UINT32 serverKey)
{
	Tank* pTank = new Tank;
	pTank->Initiate(serverKey);
	_tankTable.Insert(serverKey, pTank);
	return pTank;
}

Projectile* AllocObjectManager::CreateProjectile(UINT serverKey, Transform* pInitTransform)
{
	Projectile* pProjectile = new Projectile();
	pProjectile->Initiate(serverKey, pInitTransform);
	_projectileTable.Insert(serverKey, pProjectile);
	return pProjectile;
}

Obstacle* AllocObjectManager::CreateObstacle(UINT key, Transform* pInitTransform)
{
	
	Obstacle* pObstacle = new Obstacle();
	pObstacle->Initiate(key, pInitTransform);
	_obstacleTable.Insert(key, pObstacle);
	return pObstacle;
}

void AllocObjectManager::RemoveObject(EGameObjectKind objectKind, UINT32 key)
{
	void* ptr = nullptr;
	switch (objectKind) {
	case GAME_OBJECT_KIND_TANK:
		ptr = _tankTable.Pop(key);
		break;
	case GAME_OBJECT_KIND_PROJECTILE:
		ptr = _projectileTable.Pop(key);
		break;
	case GAME_OBJECT_KIND_OBSTACLE:
		ptr = _obstacleTable.Pop(key);
		break;
	}

	if (ptr != nullptr) {
		delete ptr;
	}
}

void AllocObjectManager::UpdateObject(EGameObjectKind objectKind, GameObject* obj)
{
	if (obj == nullptr) {
		__debugbreak();
		return;
	}

	UINT key = obj->GetID();
	GameObject* pGameObject = GetObjectPtrOrNull(objectKind, key);

	BOOL res = pGameObject->UpdateFrom(obj);
	assert(res);
}

void AllocObjectManager::UpdateObjectTransform(EGameObjectKind objectKind, UINT objId, const Transform* pTransform)
{
	GameObject* pGameObject = GetObjectPtrOrNull(objectKind, objId);
	if (pGameObject == nullptr) {
		return;
	}
	pGameObject->UpdateTransform(pTransform);
}

void AllocObjectManager::GetTransformedModelOf(EGameObjectKind objectKind, UINT key, Vertex* out_vertices, UINT* out_numVectors)
{
	GameObject* pGameObject = GetObjectPtrOrNull(objectKind, key);

	*out_numVectors = pGameObject->GetTransformedModel(out_vertices);
}

void AllocObjectManager::GetKeys(EGameObjectKind objectKind, UINT* out_keys, int* out_numKeys) const
{
	if (out_keys == nullptr || out_numKeys == nullptr) {
		__debugbreak();
		return;
	}

	int* keys = (int*)out_keys;

	switch (objectKind) {
		break;
	case GAME_OBJECT_KIND_TANK:
		_tankTable.GetIdsTo(keys, out_numKeys);
		break;
	case GAME_OBJECT_KIND_PROJECTILE:
		_projectileTable.GetIdsTo(keys, out_numKeys);
		break;
	case GAME_OBJECT_KIND_OBSTACLE:
		_obstacleTable.GetIdsTo(keys, out_numKeys);
		break;
	default:
		__debugbreak();
		break;
	}
}

void AllocObjectManager::GetAllKeys(UINT* out_keys, int* out_numKeys) const
{
	if (out_keys == nullptr || out_numKeys == nullptr) {
		__debugbreak();
		return;
	}

	int* keys = (int*)out_keys;
	int countKeys = 0;

	_tankTable.GetIdsTo(keys, &countKeys);
	*out_numKeys += countKeys;

	keys += countKeys;
	_projectileTable.GetIdsTo(keys, &countKeys);
	*out_numKeys += countKeys;

	keys += countKeys;
	_obstacleTable.GetIdsTo(keys, &countKeys);
	*out_numKeys += countKeys;

}

GameObject* AllocObjectManager::GetObjectPtrOrNull(EGameObjectKind objectKind, UINT key)
{
	GameObject* pGameObject = nullptr;

	switch (objectKind) {
	case GAME_OBJECT_KIND_TANK:
		pGameObject = (GameObject*)_tankTable.Get(key);
		break;
	case GAME_OBJECT_KIND_PROJECTILE:
		pGameObject = (GameObject*)_projectileTable.Get(key);
		break;
	case GAME_OBJECT_KIND_OBSTACLE:
		pGameObject = (GameObject*)_obstacleTable.Get(key);
		break;
	default:
		pGameObject = nullptr;
		__debugbreak();
		break;
	}

	return pGameObject;
}

int AllocObjectManager::GetCountObjects() const
{
	int countObject = 0;
	countObject += _tankTable.GetCount();
	countObject += _projectileTable.GetCount();
	countObject += _obstacleTable.GetCount();
	return countObject;
}
