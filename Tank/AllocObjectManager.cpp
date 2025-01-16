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
	ObjectID* keys = DNew ObjectID[MAX_NUM_OBJECT];
	UINT32 numCounts = 0;

	_tankTable.GetIdsTo(keys, &numCounts);
	for (int i = 0; i < numCounts; ++i) {
		ObjectID key = keys[i];
		void* ptr = _tankTable.Pop(key);
		delete ptr;
	}

	_projectileTable.GetIdsTo(keys, &numCounts);
	for (int i = 0; i < numCounts; ++i) {
		ObjectID key = keys[i];
		void* ptr = _projectileTable.Pop(key);
		delete ptr;
	}

	_obstacleTable.GetIdsTo(keys, &numCounts);
	for (int i = 0; i < numCounts; ++i) {
		ObjectID key = keys[i];
		void* ptr = _obstacleTable.Pop(key);
		delete ptr;
	}

	delete[] keys;

	_projectileTable.Terminate();
	_obstacleTable.Terminate();
	_tankTable.Terminate();
}

Tank* AllocObjectManager::CreateTank(ObjectID serverKey)
{
	Tank* pTank = DNew Tank;
	pTank->Initiate(serverKey);
	_tankTable.Insert(serverKey, pTank);
	return pTank;
}

Projectile* AllocObjectManager::CreateProjectile(ObjectID serverKey, Transform* pInitTransform)
{
	Projectile* pProjectile = DNew Projectile();
	pProjectile->Initiate(serverKey, pInitTransform);
	_projectileTable.Insert(serverKey, pProjectile);
	return pProjectile;
}

Obstacle* AllocObjectManager::CreateObstacle(ObjectID key, Transform* pInitTransform)
{
	
	Obstacle* pObstacle = DNew Obstacle();
	pObstacle->Initiate(key, pInitTransform);
	_obstacleTable.Insert(key, pObstacle);
	return pObstacle;
}

void AllocObjectManager::RemoveObject(EGameObjectKind objectKind, ObjectID key)
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

	ObjectID key = obj->GetID();
	GameObject* pGameObject = GetObjectPtrOrNull(objectKind, key);

	BOOL res = pGameObject->UpdateFrom(obj);
	assert(res);
}

void AllocObjectManager::UpdateObjectTransform(EGameObjectKind objectKind, ObjectID objId, const Transform* pTransform)
{
	GameObject* pGameObject = GetObjectPtrOrNull(objectKind, objId);
	if (pGameObject == nullptr) {
		return;
	}
	pGameObject->UpdateTransform(pTransform);
}

void AllocObjectManager::GetTransformedModelOf(EGameObjectKind objectKind, ObjectID key, Vertex* out_vertices, UINT* out_numVectors)
{
	GameObject* pGameObject = GetObjectPtrOrNull(objectKind, key);

	*out_numVectors = pGameObject->GetTransformedModel(out_vertices);
}

void AllocObjectManager::GetKeys(EGameObjectKind objectKind, ObjectID* out_keys, UINT32* out_numKeys) const
{
	if (out_keys == nullptr || out_numKeys == nullptr) {
		__debugbreak();
		return;
	}

	ObjectID* keys = out_keys;

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

void AllocObjectManager::GetAllKeys(ObjectID* out_keys, UINT32* out_numKeys) const
{
	if (out_keys == nullptr || out_numKeys == nullptr) {
		__debugbreak();
		return;
	}

	ObjectID* keys = out_keys;
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

GameObject* AllocObjectManager::GetObjectPtrOrNull(EGameObjectKind objectKind, ObjectID key)
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

void AllocObjectManager::StartTankMove(ObjectID objectId, EMOVEMENT movement)
{
	Tank* pTank = (Tank*)_tankTable.Get(objectId);
	if (pTank == nullptr) {
		return;
	}

	pTank->StartMove(movement);
}

void AllocObjectManager::EndTankMove(ObjectID objectId, EMOVEMENT movement, const Transform* pTransform)
{
	Tank* pTank = (Tank*)_tankTable.Get(objectId);
	if (pTank == nullptr) {
		return;
	}

	pTank->EndMove(movement, pTransform);
}

void AllocObjectManager::StartTankRotate(ObjectID objectId, EROTATION rotation)
{
	Tank* pTank = (Tank*)_tankTable.Get(objectId);
	if (pTank == nullptr) {
		return;
	}

	pTank->StartRotate(rotation);
}

void AllocObjectManager::EndTankRotate(ObjectID objectId, EROTATION rotation, const Transform* pTransform)
{
	Tank* pTank = (Tank*)_tankTable.Get(objectId);
	if (pTank == nullptr) {
		return;
	}

	pTank->EndRotate(rotation, pTransform);
}
