#include "ObjectManager.h"
#include "Tank.h"
#include "GameEvent.h"
#include "Projectile.h"
#include "Global.h"

void ObjectManager::Initiate()
{
	_unusedObjectIdQueue.Initiate(sizeof(ObjectID), NUM_OBJECTS_MAX);
	
	_tankTable.Initiate(NUM_OBJECTS_MAX);
	_tankTableByOwner.Initiate(NUM_OBJECTS_MAX);

	_projectileTable.Initiate(NUM_OBJECTS_MAX);
	_obstacleTable.Initiate(NUM_OBJECTS_MAX);

	for (ObjectID i = 0; i < NUM_OBJECTS_MAX; ++i) {
		_unusedObjectIdQueue.Push(&i);
	}
}

void ObjectManager::Terminate()
{
	ObjectID* keys = new ObjectID[NUM_OBJECTS_MAX];
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

	_obstacleTable.Terminate();
	_projectileTable.Terminate();
	_tankTableByOwner.Terminate();
	_tankTable.Terminate();
	_unusedObjectIdQueue.Terminate();
}

EGameObjectKind ObjectManager::FindObjectKindByID(ObjectID id)
{
	ObjectID objectIDs[NUM_OBJECTS_MAX];
	int numObjects = 0;
	_tankTable.GetIdsTo(objectIDs, &numObjects);
	for (int i = 0; i < numObjects; ++i) {
		if (objectIDs[i] == id) {
			return GAME_OBJECT_KIND_TANK;
		}
	}

	_projectileTable.GetIdsTo(objectIDs, &numObjects);
	for (int i = 0; i < numObjects; ++i) {
		if (objectIDs[i] == id) {
			return GAME_OBJECT_KIND_PROJECTILE;
		}
	}

	_obstacleTable.GetIdsTo(objectIDs, &numObjects);
	for (int i = 0; i < numObjects; ++i) {
		if (objectIDs[i] == id) {
			return GAME_OBJECT_KIND_OBSTACLE;
		}
	}
	
	return GAME_OBJECT_KIND_INVALID;
}

Tank* ObjectManager::CreateTank(UserDBIndex ownerIndex)
{
	assert(_unusedObjectIdQueue.GetCount() != 0);
	ObjectID objectId;
	_unusedObjectIdQueue.TryPopTo(&objectId);
	assert(objectId != UINT16_MAX);

	Collider* pCollider = g_pCollisionManager->GetNewColliderPtr(Tank::COLLIDER_RADIUS, objectId);
	Tank* pTank = new Tank(objectId, ownerIndex, pCollider);
	bool res = _tankTable.Insert(objectId, pTank);
	assert(res);
	res = _tankTableByOwner.Insert(ownerIndex, pTank);
	assert(res);

	return pTank;
}

void ObjectManager::RemoveTank(ObjectID objectId, UserDBIndex ownerId)
{
	Tank* pTank = (Tank*)_tankTable.Pop(objectId);
	assert(pTank != nullptr);
	pTank = (Tank*)_tankTableByOwner.Pop(ownerId);
	assert(pTank != nullptr);
	auto res = _unusedObjectIdQueue.Push(&objectId);
	assert(res);
	delete pTank;
}

Projectile* ObjectManager::CreateProjectile(UserDBIndex ownerId, Transform* pTransform)
{
	assert(_unusedObjectIdQueue.GetCount() != 0);
	UINT16 objectId;
	_unusedObjectIdQueue.TryPopTo(&objectId);
	assert(objectId != UINT16_MAX);

	Collider* pCollider = g_pCollisionManager->GetNewColliderPtr(Projectile::COLLIDER_RADIUS, objectId);

	Projectile* pProjectile = new Projectile;
	pProjectile->Initiate(objectId, pTransform, ownerId, pCollider);
	
	bool res = _projectileTable.Insert(objectId, pProjectile);
	assert(res);
	return pProjectile;
}

void ObjectManager::RemoveProjectile(ObjectID objectId)
{
	Projectile* pProjectile = (Projectile*)_projectileTable.Pop(objectId);
	assert(pProjectile != nullptr);
	auto res = _unusedObjectIdQueue.Push(&objectId);
	assert(res);
	delete pProjectile;
}

Tank* ObjectManager::GetTankByObjectId(ObjectID objectId)
{
	Tank* pTank = (Tank*)_tankTable.Get(objectId);
	assert(pTank != nullptr);
	return pTank;
}

Tank* ObjectManager::GetTankByOwnerId(UserDBIndex ownerId)
{
	Tank* pTank = (Tank*)_tankTableByOwner.Get(ownerId);
	return pTank;
}

void ObjectManager::StartTankMove(UserDBIndex ownerId, EMOVEMENT movement)
{
	Tank* pTank = (Tank*)_tankTableByOwner.Get(ownerId);
	if (pTank == nullptr) {
		return;
	}

	pTank->StartMove(movement);
}

void ObjectManager::EndTankMove(UserDBIndex ownerId, EMOVEMENT movement)
{
	Tank* pTank = (Tank*)_tankTableByOwner.Get(ownerId);
	if (pTank == nullptr) {
		return;
	}

	pTank->EndMove(movement);
}

void ObjectManager::StartTankRotate(UserDBIndex ownerId, EROTATION rotation)
{
	Tank* pTank = (Tank*)_tankTableByOwner.Get(ownerId);
	if (pTank == nullptr) {
		return;
	}

	pTank->StartRotate(rotation);
}

void ObjectManager::EndTankRotate(UserDBIndex ownerId, EROTATION rotation)
{
	Tank* pTank = (Tank*)_tankTableByOwner.Get(ownerId);
	if (pTank == nullptr) {
		return;
	}

	pTank->EndRotate(rotation);
}

void ObjectManager::UpdateTankTransform(ObjectID objectId, const Transform* pTransform)
{
	Tank* pTank = (Tank*)_tankTable.Get(objectId);
	assert(pTank != nullptr);

	pTank->UpdateTransform(pTransform);
}

void ObjectManager::UpdateTankTransform(UserDBIndex ownerId, const Transform* pTransform)
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
	ObjectID* keys = new ObjectID[NUM_OBJECTS_MAX];
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

void ObjectManager::RemoveObject(EGameObjectKind objectKind, ObjectID key)
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

void ObjectManager::GetKeys(EGameObjectKind objectKind, ObjectID* out_keys, int* out_numKeys) const
{
	if (out_keys == nullptr || out_numKeys == nullptr) {
		__debugbreak();
		return;
	}

	ObjectID* keys = (ObjectID*)out_keys;

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

GameObject* ObjectManager::GetObjectPtrOrNull(EGameObjectKind objectKind, ObjectID key)
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

GameObject* ObjectManager::GetObjectPtrOrNull(ObjectID id)
{
	ObjectID objectIDs[NUM_OBJECTS_MAX];
	int numObjects = 0;
	_tankTable.GetIdsTo(objectIDs, &numObjects);
	for (int i = 0; i < numObjects; ++i) {
		if (objectIDs[i] == id) {
			return (GameObject*)_tankTable.Get(id);
		}
	}

	_projectileTable.GetIdsTo(objectIDs, &numObjects);
	for (int i = 0; i < numObjects; ++i) {
		if (objectIDs[i] == id) {
			return (GameObject*)_projectileTable.Get(id);;
		}
	}

	_obstacleTable.GetIdsTo(objectIDs, &numObjects);
	for (int i = 0; i < numObjects; ++i) {
		if (objectIDs[i] == id) {
			return (GameObject*)_obstacleTable.Get(id);;
		}
	}

	return nullptr;
}


