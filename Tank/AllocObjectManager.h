#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "Tank.h"
#include "Projectile.h"
#include "DotObject.h"
#include "Obstacle.h"


#include "LinearQueue.h"
#include "PointerTable.h"

enum EGameObjectKind: UINT16;

class AllocObjectManager
{
public:

	void Initiate();
	void Terminate();

	Tank* CreateTank(ObjectID serverKey);
	Projectile* CreateProjectile(ObjectID serverKey, Transform* pInitTransform);
	Obstacle* CreateObstacle(ObjectID key, Transform* pInitTransform);

	void RemoveObject(EGameObjectKind objectKind, ObjectID key);

	void UpdateObject(EGameObjectKind objectKind, GameObject* obj);

	void UpdateObjectTransform(EGameObjectKind objectKind, ObjectID objId, const Transform* pTransform);

	void GetTransformedModelOf(EGameObjectKind objectKind, ObjectID key, Vertex* out_vertices, UINT* out_numVectors);

	void GetKeys(EGameObjectKind objectKind, ObjectID* out_keys, int* out_numKeys) const;
	void GetAllKeys(ObjectID* out_keys, int* out_numKeys) const;

	GameObject* GetObjectPtrOrNull(EGameObjectKind objectKind, ObjectID key);

	int GetCountObjects() const;


	void StartTankMove(ObjectID objectId, EMOVEMENT movement);
	void EndTankMove(ObjectID objectId, EMOVEMENT movement, const Transform* pTransform);

	void StartTankRotate(ObjectID objectId, EROTATION rotation);
	void EndTankRotate(ObjectID objectId, EROTATION rotation, const Transform* pTransform);

private:
	PointerTable16 _tankTable;
	PointerTable16 _projectileTable;
	PointerTable16 _obstacleTable;
};
