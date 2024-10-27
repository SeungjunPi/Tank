#include "ObjectManager.h"
#include "Tank.h"
#include "GameEvent.h"
#include "Projectile.h"

void ObjectManager::Initiate()
{
	_unusedObjectIdQueue.Initiate(sizeof(UINT16), NUM_OBJECTS_MAX);
	
	_tankTable.Initiate(NUM_OBJECTS_MAX);
	_tankTableByOwner.Initiate(NUM_OBJECTS_MAX);

	_projectileTable.Initiate(NUM_OBJECTS_MAX);
	_obstacleTable.Initiate(NUM_OBJECTS_MAX);

	for (UINT16 i = 0; i < NUM_OBJECTS_MAX; ++i) {
		_unusedObjectIdQueue.Push(&i);
	}
}

void ObjectManager::Terminate()
{
	int* keys = new int[NUM_OBJECTS_MAX];
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
	_tankTableByOwner.Terminate();
	_tankTable.Terminate();
	_unusedObjectIdQueue.Terminate();
}

Tank* ObjectManager::CreateTank(UINT32 ownerId)
{
	assert(_unusedObjectIdQueue.GetCount() != 0);
	UINT16 objectId;
	_unusedObjectIdQueue.TryPopTo(&objectId);
	assert(objectId != UINT16_MAX);

	Tank* pTank = new Tank(objectId, ownerId);
	bool res = _tankTable.Insert(objectId, pTank);
	assert(res);
	res = _tankTableByOwner.Insert(ownerId, pTank);
	assert(res);

	return pTank;
}

void ObjectManager::RemoveTank(UINT16 objectId, UINT32 ownerId)
{
	Tank* pTank = (Tank*)_tankTable.Pop(objectId);
	assert(pTank != nullptr);
	pTank = (Tank*)_tankTableByOwner.Pop(ownerId);
	assert(pTank != nullptr);
	auto res = _unusedObjectIdQueue.Push(&objectId);
	assert(res);
	delete pTank;
}

Projectile* ObjectManager::CreateProjectile(UINT32 ownerId, Transform* pTransform)
{
	assert(_unusedObjectIdQueue.GetCount() != 0);
	UINT16 objectId;
	_unusedObjectIdQueue.TryPopTo(&objectId);
	assert(objectId != UINT16_MAX);

	Projectile* pProjectile = new Projectile;
	pProjectile->Initiate(objectId, pTransform);
	
	bool res = _projectileTable.Insert(objectId, pProjectile);
	assert(res);
	return pProjectile;
}

void ObjectManager::RemoveProjectile(UINT16 objectId)
{
	Projectile* pProjectile = (Projectile*)_projectileTable.Pop(objectId);
	assert(pProjectile != nullptr);
	auto res = _unusedObjectIdQueue.Push(&objectId);
	assert(res);
	delete pProjectile;
}

Tank* ObjectManager::GetTank(UINT16 objectId)
{
	Tank* pTank = (Tank*)_tankTable.Get(objectId);
	assert(pTank != nullptr);
	return pTank;
}

Tank* ObjectManager::GetTank(UINT32 ownerId)
{
	Tank* pTank = (Tank*)_tankTableByOwner.Get(ownerId);
	return pTank;
}

void ObjectManager::StartTankMove(UINT32 ownerId, EMOVEMENT movement)
{
	Tank* pTank = (Tank*)_tankTableByOwner.Get(ownerId);
	if (pTank == nullptr) {
		return;
	}

	pTank->StartMove(movement);
}

void ObjectManager::EndTankMove(UINT32 ownerId, EMOVEMENT movement)
{
	Tank* pTank = (Tank*)_tankTableByOwner.Get(ownerId);
	if (pTank == nullptr) {
		return;
	}

	pTank->EndMove(movement);
}

void ObjectManager::StartTankRotate(UINT32 ownerId, EROTATION rotation)
{
	Tank* pTank = (Tank*)_tankTableByOwner.Get(ownerId);
	if (pTank == nullptr) {
		return;
	}

	pTank->StartRotate(rotation);
}

void ObjectManager::EndTankRotate(UINT32 ownerId, EROTATION rotation)
{
	Tank* pTank = (Tank*)_tankTableByOwner.Get(ownerId);
	if (pTank == nullptr) {
		return;
	}

	pTank->EndRotate(rotation);
}

void ObjectManager::UpdateTankTransform(UINT16 objectId, const Transform* pTransform)
{
	Tank* pTank = (Tank*)_tankTable.Get(objectId);
	assert(pTank != nullptr);

	pTank->UpdateTransform(pTransform);
}

void ObjectManager::UpdateTankTransform(UINT32 ownerId, const Transform* pTransform)
{
	Tank* pTank = (Tank*)_tankTableByOwner.Get(ownerId);
	if (pTank == nullptr) {
		return;
	}

	pTank->UpdateTransform(pTransform);
}

UINT16 ObjectManager::GetCountObjects() const
{
	int countTank = _tankTable.GetCount();
	
	return (UINT16)countTank;
}

void ObjectManager::CopySnapshot(PACKET_OBJECT_INFO* dst)
{
	int* keys = new int[NUM_OBJECTS_MAX];
	int numCounts = 0;
	_tankTable.GetIdsTo(keys, &numCounts);

	PACKET_OBJECT_INFO* pObjInfo = dst;
	for (int i = 0; i < numCounts; ++i) {
		int key = keys[i];
		Tank* pTank = (Tank*)_tankTable.Get(key);

		pObjInfo->kind = GAME_OBJECT_KIND_TANK;
		pObjInfo->objectId = pTank->GetID();
		pObjInfo->ownerId = pTank->GetOwnerId();
		memcpy(&pObjInfo->transform, pTank->GetTransformPtr(), sizeof(Transform));

		++pObjInfo;
	}
}

void ObjectManager::RemoveObject(EGameObjectKind objectKind, UINT32 key)
{
	void* ptr = nullptr;
	switch (objectKind) {
	case GAME_OBJECT_KIND_TANK:
	{
		ptr = _tankTable.Pop(key);
		UINT32 ownerId = ((Tank*)ptr)->GetOwnerId();
		_tankTableByOwner.Pop(ownerId);
		break;
	}
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

	_unusedObjectIdQueue.Push(&key);
}

void ObjectManager::GetKeys(EGameObjectKind objectKind, UINT* out_keys, int* out_numKeys) const
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

GameObject* ObjectManager::GetObjectPtrOrNull(EGameObjectKind objectKind, UINT key)
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


