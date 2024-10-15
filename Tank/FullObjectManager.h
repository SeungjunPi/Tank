#pragma once

#include "GameObject.h"
#include "Tank.h"
#include "Projectile.h"
#include "DotObject.h"
#include "SinglePageMemoryPool.h"


class FullObjectManager
{
public:
	void Initiate();
	void Terminate();

	Tank* CreateTank();
	Projectile* CreateProjectile(Transform* initTransform);

	void RemoveObject(UINT key);

	void UpdateObject(GameObject* obj);

	void GetTransformedModelOf(UINT key, Vertex* out_vertices, UINT* out_numVectors);

	void GetKeys(UINT* out_keys, int* out_numKeys) const;

	GameObject* GetObjectPtrOrNull(UINT key);

	int GetCountObjects() const;

private:
	UINT* _usingKeys = nullptr;
	UINT _key = 0;

	int _numObjects = 0;
	GameObject** _pObjects = nullptr;
	int _numTanks = 0;
	int _numProecjtile = 0;

	int _numDirtyObjects = 0;
	UINT* _dirtyObjects = nullptr;

	ISinglePageMemoryPool* _tankPool;
	ISinglePageMemoryPool* _projectilePool;
};
