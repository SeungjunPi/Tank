#pragma once

#include "GameObject.h"
#include "PointerTable.h"
#include "LinearQueue.h"

class Tank;
class Projectile;
struct PACKET_OBJECT_INFO;

#define NUM_OBJECTS_MAX (4096)

// 4096
class ObjectManager
{
public:
	void Initiate();
	void Terminate();

	Tank* CreateTank(UINT32 ownerId);
	void RemoveTank(UINT16 objectId, UINT32 ownerId);

	Projectile* CreateProjectile(UINT32 ownerId, Transform* pTransform);
	void RemoveProjectile(UINT16 objectId);

	Tank* GetTankByObjectId(UINT16 objectId);
	Tank* GetTankByOwnerId(UINT32 ownerId);

	void StartTankMove(UINT32 ownerId, EMOVEMENT movement);
	void EndTankMove(UINT32 ownerId, EMOVEMENT movement);

	void StartTankRotate(UINT32 ownerId, EROTATION rotation);
	void EndTankRotate(UINT32 ownerId, EROTATION rotation);



	void UpdateTankTransform(UINT16 objectId, const Transform* pTransform);
	void UpdateTankTransform(UINT32 ownerId, const Transform* pTransform);

	UINT16 GetCountObjects() const;
	void CopySnapshot(PACKET_OBJECT_INFO* dst);

	void RemoveObject(EGameObjectKind objectKind, UINT32 key);
	void GetKeys(EGameObjectKind objectKind, UINT* out_keys, int* out_numKeys) const;
	GameObject* GetObjectPtrOrNull(EGameObjectKind objectKind, UINT key);
	

private:
	LinearQueue _unusedObjectIdQueue;
	
	PointerTable _tankTable;
	PointerTable _tankTableByOwner;

	PointerTable _projectileTable;

	PointerTable _obstacleTable;
};
