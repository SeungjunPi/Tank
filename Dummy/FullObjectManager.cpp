
#include "FullObjectManager.h"

static const int MAX_NUM_OBJECTS = 1024;
static const int MAX_KEY = 0x00100000;

void FullObjectManager::Initiate()
{
	_usingKeys = new UINT[MAX_NUM_OBJECTS];
	ZeroMemory(_usingKeys, sizeof(UINT) * MAX_NUM_OBJECTS);
	_dirtyObjects = new UINT[MAX_NUM_OBJECTS];

	_pObjects = new GameObject*[MAX_KEY];

	CreateSinglePageMemoryPool(&_tankPool);
	_tankPool->Initiate(sizeof(Tank), MAX_NUM_OBJECTS); // separate.. 

	CreateSinglePageMemoryPool(&_projectilePool);
	_projectilePool->Initiate(sizeof(Projectile), MAX_NUM_OBJECTS);
}

void FullObjectManager::Terminate()
{
	_projectilePool->Terminate();
	DeleteSinglePageMemoryPool(_projectilePool);
	_projectilePool = nullptr;

	_tankPool->Terminate();
	DeleteSinglePageMemoryPool(_tankPool);
	_tankPool = nullptr;
	
	delete[] _pObjects;
	delete[] _dirtyObjects;
	delete[] _usingKeys;

}


Tank* FullObjectManager::CreateTank()
{
	Tank* pTank = (Tank*)_tankPool->TryGetPointer();
	UINT newKey = _key | Tank::OBJ_KEY;
	pTank->Initiate(newKey);
	_usingKeys[_numObjects] = newKey;

	_pObjects[newKey] = pTank;

	_key++;
	_numObjects++;
	_numTanks++;

	return pTank;
}

Projectile* FullObjectManager::CreateProjectile(Transform* initTransform)
{
	Projectile* pProjectile = (Projectile*)_projectilePool->TryGetPointer();
	UINT newKey = _key | Projectile::OBJ_KEY;
	Vector3 v;
	pProjectile->Initiate(newKey, initTransform, v);
	_usingKeys[_numObjects] = newKey;

	_pObjects[newKey] = pProjectile;
	_key++;
	_numObjects++;
	_numProecjtile++;

	return pProjectile;
}

void FullObjectManager::RemoveObject(UINT key)
{
	// Todo: valitate key
	ZeroMemory(_pObjects + key, sizeof(GameObject));

	for (int i = 0; i < _numObjects; ++i) {
		if (_usingKeys[i] == key) {
			--_numObjects;
			_usingKeys[i] = _usingKeys[_numObjects];

		}
	}
}

void FullObjectManager::UpdateObject(GameObject* obj)
{
	if (obj == nullptr) {
		__debugbreak();
		return;
	}

	UINT key = obj->GetID();
	BOOL res = _pObjects[key]->UpdateFrom(obj); // 
}

void FullObjectManager::GetTransformedModelOf(UINT key, Vertex* out_vertices, UINT* out_numVectors)
{
	*out_numVectors = _pObjects[key]->GetTransformedModel(out_vertices);
}

void FullObjectManager::GetKeys(UINT* out_keys, int* out_numKeys) const
{
	if (out_keys == nullptr) {
		__debugbreak();
		return;
	}

	for (int i = 0; i < _numObjects; ++i) {
		out_keys[i] = _usingKeys[i];
	}
	*out_numKeys = _numObjects;
}

GameObject* FullObjectManager::GetObjectPtrOrNull(UINT key)
{
	return _pObjects[key];
}

int FullObjectManager::GetCountObjects() const
{
	return _numObjects;
}
