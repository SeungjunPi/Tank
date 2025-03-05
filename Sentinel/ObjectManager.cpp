#include "ObjectManager.h"
#include "Tank.h"
#include "ServerPacketHandler.h"
#include "Projectile.h"
#include "Global.h"
#include "StaticData.h"
#include "IStableFlow.h"

void ObjectManager::Initiate()
{
	_unusedObjectIdQueue.Initiate(sizeof(ObjectID::key), NUM_OBJECTS_MAX);
	
	_tankTable.Initiate(NUM_OBJECTS_MAX);
	_tankTableByOwner.Initiate(NUM_OBJECTS_MAX);

	_projectileTable.Initiate(NUM_OBJECTS_MAX);
	_obstacleTable.Initiate(NUM_OBJECTS_MAX);

	for (ObjectKey i = 0; i < NUM_OBJECTS_MAX; ++i) {
		_unusedObjectIdQueue.Push(&i);
	}
}

void ObjectManager::Terminate()
{
	UINT32* indexes = DNew UINT32[NUM_OBJECTS_MAX];
	UINT32 numCounts = 0;


	_tankTable.GetIdsTo(indexes, &numCounts);
	for (UINT32 i = 0; i < numCounts; ++i) {
		UINT32 key = indexes[i];
		void* ptr = _tankTable.Pop(key);
		delete ptr;
	}

	_projectileTable.GetIdsTo(indexes, &numCounts);
	for (UINT32 i = 0; i < numCounts; ++i) {
		UINT32 key = indexes[i];
		void* ptr = _projectileTable.Pop(key);
		delete ptr;
	}

	_obstacleTable.GetIdsTo(indexes, &numCounts);
	for (UINT32 i = 0; i < numCounts; ++i) {
		UINT32 key = indexes[i];
		void* ptr = _obstacleTable.Pop(key);
		delete ptr;
	}
	
	delete[] indexes;

	_obstacleTable.Terminate();
	_projectileTable.Terminate();
	_tankTableByOwner.Terminate();
	_tankTable.Terminate();
	_unusedObjectIdQueue.Terminate();
}

Tank* ObjectManager::CreateTank(UserDBIndex ownerIndex)
{
	assert(_unusedObjectIdQueue.GetCount() != 0);
	ObjectID objectId;
	objectId.type = GAME_OBJECT_TYPE_TANK;
	_unusedObjectIdQueue.TryPopTo(&objectId.key);
	assert(objectId.key != INVALID_OBJECT_KEY);

	Tank* pTank = DNew Tank(objectId, ownerIndex);
	Collider* pCollider = g_pStableFlow->GetNewColliderPtr(pTank, pTank->GetPhysicalComponentPtr());
	pTank->AttachCollider(pCollider);
	bool res = _tankTable.Insert(objectId.key, pTank);
	assert(res);
	res = _tankTableByOwner.Insert(ownerIndex, pTank);
	assert(res);

	return pTank;
}

void ObjectManager::RemoveTank(ObjectID objectId, UserDBIndex ownerId)
{
	Tank* pTank = (Tank*)_tankTable.Pop(objectId.key);
	assert(pTank != nullptr);
	pTank = (Tank*)_tankTableByOwner.Pop(ownerId);
	assert(pTank != nullptr);

	Collider* pCollider = pTank->GetColliderPtr();
	g_pStableFlow->ReturnCollider(pCollider);

	auto res = _unusedObjectIdQueue.Push(&objectId.key);
	assert(res);
	delete pTank;
}

Projectile* ObjectManager::CreateProjectile(UserDBIndex ownerId, const Transform* pTransform)
{
	assert(_unusedObjectIdQueue.GetCount() != 0);
	ObjectID objectId;
	_unusedObjectIdQueue.TryPopTo(&objectId.key);
	objectId.type = GAME_OBJECT_TYPE_PROJECTILE;
	assert(objectId.key != INVALID_OBJECT_KEY);

	Projectile* pProjectile = DNew Projectile;
	Collider* pCollider = g_pStableFlow->GetNewColliderPtr(pProjectile, pProjectile->GetPhysicalComponentPtr());
	pProjectile->AttachCollider(pCollider);
	pProjectile->Initiate(objectId, pTransform, ownerId);
	bool res = _projectileTable.Insert(objectId.key, pProjectile);
	assert(res);
	return pProjectile;
}

void ObjectManager::RemoveProjectile(ObjectID objectId)
{
	Projectile* pProjectile = (Projectile*)_projectileTable.Pop(objectId.key);
	assert(pProjectile != nullptr);

	Collider* pCollider = pProjectile->GetColliderPtr();
	g_pStableFlow->ReturnCollider(pCollider);

	auto res = _unusedObjectIdQueue.Push(&objectId.key);
	assert(res);
	delete pProjectile;
}

Tank* ObjectManager::GetTankByObjectId(ObjectID objectId)
{
	Tank* pTank = (Tank*)_tankTable.Get(objectId.key);
	assert(pTank != nullptr);
	return pTank;
}

Tank* ObjectManager::GetTankByOwnerId(UserDBIndex ownerId)
{
	Tank* pTank = (Tank*)_tankTableByOwner.Get(ownerId);
	return pTank;
}

UINT32 ObjectManager::GetCountObjects() const
{
	UINT32 countTank = _tankTable.GetCount();
	
	return countTank;
}

void ObjectManager::CopySnapshot(PACKET_OBJECT_INFO* dst)
{
	UINT32* keys = DNew UINT32[NUM_OBJECTS_MAX];
	UINT32 numCounts = 0;
	_tankTable.GetIdsTo(keys, &numCounts);

	PACKET_OBJECT_INFO* pObjInfo = dst;
	for (UINT32 i = 0; i < numCounts; ++i) {
		int key = keys[i];
		Tank* pTank = (Tank*)_tankTable.Get(key);
		
		pObjInfo->kind = GAME_OBJECT_TYPE_TANK;
		pObjInfo->objectId = pTank->GetID();
		pObjInfo->ownerId = pTank->GetOwnerId();
		memcpy(&pObjInfo->transform, pTank->GetTransformPtr(), sizeof(Transform));

		++pObjInfo;
	}

	delete[] keys;
}

void ObjectManager::RemoveObject(ObjectID objectID)
{
	
	switch (objectID.type) {
	case GAME_OBJECT_TYPE_TANK:
	{
		Tank* pTank = (Tank*)_tankTable.Get(objectID.key);
		UINT32 ownerId = pTank->GetOwnerId();
		RemoveTank(objectID, ownerId);
		break;
	}
	case GAME_OBJECT_TYPE_PROJECTILE:
		RemoveProjectile(objectID);
		break;
	case GAME_OBJECT_TYPE_OBSTACLE:
		// TODO: remove Obstacle
		break;
	}

}

void ObjectManager::GetKeys(EGameObjectType objectKind, UINT32* out_keys, UINT32* out_numKeys) const
{
	if (out_keys == nullptr || out_numKeys == nullptr) {
		__debugbreak();
		return;
	}

	switch (objectKind) {
		break;
	case GAME_OBJECT_TYPE_TANK:
		_tankTable.GetIdsTo(out_keys, out_numKeys);
		break;
	case GAME_OBJECT_TYPE_PROJECTILE:
		_projectileTable.GetIdsTo(out_keys, out_numKeys);
		break;
	case GAME_OBJECT_TYPE_OBSTACLE:
		_obstacleTable.GetIdsTo(out_keys, out_numKeys);
		break;
	default:
		__debugbreak();
		break;
	}
}

GameObject* ObjectManager::GetObjectPtrOrNull(EGameObjectType objectKind, ObjectKey objectKey)
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

GameObject* ObjectManager::GetObjectPtrOrNull(ObjectID objectID)
{
	return GetObjectPtrOrNull(objectID.type, objectID.key);
}

void ObjectManager::SetObjectInputState(ObjectID objectID, PlayerInputState inputState)
{
	if (objectID.type != GAME_OBJECT_TYPE_TANK) {
		__debugbreak();
	}

	Tank* pTank = (Tank*)_tankTable.Get(objectID.key);
	if (pTank == nullptr) {
		return;
	}
	pTank->AdvancePlayerInput(inputState);
}
