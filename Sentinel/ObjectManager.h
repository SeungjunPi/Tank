#pragma once

#include "GameObject.h"
#include "PointerTable.h"
#include "LinearQueue.h"
#include "SentinelAliases.h"
#include "CollisionManager.h"

class Tank;
class Projectile;
struct PACKET_OBJECT_INFO;

#define NUM_OBJECTS_MAX (4096) // this number should be less than UINT16_MAX

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



	void UpdateTankTransformByObjectID(ObjectID objectID, const Transform* pTransform);
	void UpdateTankTransformByOwnerID(UserDBIndex ownerId, const Transform* pTransform);

	UINT32 GetCountObjects() const;
	void CopySnapshot(PACKET_OBJECT_INFO* dst);

	void RemoveObject(ObjectID objectID);
	void GetKeys(EGameObjectType objectKind, UINT32* out_keys, UINT32* out_numKeys) const;
	GameObject* GetObjectPtrOrNull(ObjectID objectID);
	

private:
	LinearQueue _unusedObjectIdQueue;
	
	PointerTable32 _tankTable;
	PointerTable32 _tankTableByOwner;

	PointerTable32 _projectileTable;

	PointerTable32 _obstacleTable;
};
