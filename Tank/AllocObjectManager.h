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

	Tank* CreateTank(UINT32 serverKey);
	Projectile* CreateProjectile(UINT serverKey, Transform* pInitTransform);
	Obstacle* CreateObstacle(UINT key, Transform* pInitTransform);

	void RemoveObject(EGameObjectKind objectKind, UINT32 key);

	void UpdateObject(EGameObjectKind objectKind, GameObject* obj);

	void UpdateObjectTransform(EGameObjectKind objectKind, UINT objId, const Transform* pTransform);

	void GetTransformedModelOf(EGameObjectKind objectKind, UINT key, Vertex* out_vertices, UINT* out_numVectors);

	void GetKeys(EGameObjectKind objectKind, UINT* out_keys, int* out_numKeys) const;
	void GetAllKeys(UINT* out_keys, int* out_numKeys) const;

	GameObject* GetObjectPtrOrNull(EGameObjectKind objectKind, UINT key);

	int GetCountObjects() const;


	void StartTankMove(UINT32 objectId, EMOVEMENT movement);
	void EndTankMove(UINT32 objectId, EMOVEMENT movement, const Transform* pTransform);

	void StartTankRotate(UINT32 objectId, EROTATION rotation);
	void EndTankRotate(UINT32 objectId, EROTATION rotation, const Transform* pTransform);

private:
	PointerTable _tankTable;
	PointerTable _projectileTable;
	PointerTable _obstacleTable;
};
