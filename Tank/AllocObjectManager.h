#pragma once
#include "TankPch.h"
#include "GameObject.h"
#include "Tank.h"
#include "Projectile.h"
#include "DotObject.h"
#include "Obstacle.h"


#include "LinearQueue.h"
#include "PointerTable.h"

enum EGameObjectType: UINT16;

class AllocObjectManager
{
public:

	void Initiate();
	void Terminate();

	Tank* CreateTank(ObjectID objectID, UserDBIndex ownerIndex);
	Projectile* CreateProjectile(ObjectID objectID, Transform* pInitTransform, UserDBIndex ownerIndex);
	Obstacle* CreateObstacle(ObjectID objectID, Transform* pInitTransform, UserDBIndex ownerIndex);

	void RemoveObject(EGameObjectType objectKind, ObjectKey objectKey);
	void RemoveObject(ObjectID objectID);

	void UpdateObject(GameObject* obj);

	void UpdateObjectTransform(ObjectID objId, const Transform* pTransform);

	void GetTransformedModelOf(EGameObjectType objectKind, ObjectKey objectID, Vertex* out_vertices, UINT* out_numVectors);
	void GetTransformedModelOf(ObjectID objectID, Vertex* out_vertices, UINT* out_numVectors);

	void GetKeys(EGameObjectType objectKind, UINT32* out_keys, UINT32* out_numKeys) const;
	void GetAllKeys(UINT32* out_keys, UINT32* out_numKeys) const;

	GameObject* GetObjectPtrOrNull(EGameObjectType objectKind, ObjectKey objectKey);
	GameObject* GetObjectPtrOrNull(ObjectID objectID);

	int GetCountObjects() const;


	void StartTankMove(ObjectID objectId, EMOVEMENT movement);
	void EndTankMove(ObjectID objectId, EMOVEMENT movement, const Transform* pTransform);

	void StartTankRotate(ObjectID objectId, EROTATION rotation);
	void EndTankRotate(ObjectID objectId, EROTATION rotation, const Transform* pTransform);

private:
	PointerTable32 _tankTable;
	PointerTable32 _projectileTable;
	PointerTable32 _obstacleTable;
};
