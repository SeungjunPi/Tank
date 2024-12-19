#pragma once

#include "GameObject.h"
#include "PointerTable.h"
#include "LinearQueue.h"
#include "SentinelAliases.h"
#include "CollisionManager.h"

class Tank;
class Projectile;
struct PACKET_OBJECT_INFO;

#define NUM_OBJECTS_MAX (4096)

class ObjectManager
{
public:
	void Initiate();
	void Terminate();
	
	Tank* CreateTank(UserDBIndex ownerId);
	void RemoveTank(ObjectID objectId, UserDBIndex ownerId);

	Projectile* CreateProjectile(UserDBIndex ownerId, Transform* pTransform);
	void RemoveProjectile(ObjectID objectId);

	Tank* GetTankByObjectId(ObjectID objectId);
	Tank* GetTankByOwnerId(UserDBIndex ownerId);

	void StartTankMove(UserDBIndex ownerId, EMOVEMENT movement);
	void EndTankMove(UserDBIndex ownerId, EMOVEMENT movement);

	void StartTankRotate(UserDBIndex ownerId, EROTATION rotation);
	void EndTankRotate(UserDBIndex ownerId, EROTATION rotation);



	void UpdateTankTransform(ObjectID objectId, const Transform* pTransform);
	void UpdateTankTransform(UserDBIndex ownerId, const Transform* pTransform);

	UINT16 GetCountObjects() const;
	void CopySnapshot(PACKET_OBJECT_INFO* dst);

	void RemoveObject(EGameObjectKind objectKind, ObjectID key);
	void GetKeys(EGameObjectKind objectKind, ObjectID* out_keys, int* out_numKeys) const;
	GameObject* GetObjectPtrOrNull(EGameObjectKind objectKind, ObjectID key);
	

private:
	LinearQueue _unusedObjectIdQueue;
	
	PointerTable16 _tankTable;
	PointerTable32 _tankTableByOwner;

	PointerTable16 _projectileTable;

	PointerTable16 _obstacleTable;
};
