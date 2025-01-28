

#include "ObjectManager.h"
#include "SinglePageMemoryPool.h"

void ObjectManager::Initiate(UINT keyMax)
{
	_usingKeys = new UINT[keyMax + 1];
	ZeroMemory(_usingKeys, sizeof(UINT) * (keyMax + 1));
	_objects = new GameObject[keyMax + 1];
	_keyMax = keyMax;
}

void ObjectManager::Terminate()
{
	delete[] _usingKeys;
	delete[] _objects;
}

void ObjectManager::AddObject(GameObject* obj)
{
	if (obj == nullptr) {
		__debugbreak();
		return;
	}

	UINT key = obj->GetID();
	memcpy(_objects + key, obj, sizeof(GameObject));
	_usingKeys[_countObjects] = key;
	++_countObjects;
}

void ObjectManager::RemoveObject(GameObject* obj)
{
	if (obj == nullptr) {
		__debugbreak();
		return;
	}
	UINT key = obj->GetID();
	
	ObjectManager::RemoveObject(key);
}

void ObjectManager::RemoveObject(UINT key)
{
	if (key > _keyMax) {
		__debugbreak();
		return;
	}

	ZeroMemory(_objects + key, sizeof(GameObject));

	for (int i = 0; i < _countObjects; ++i) {
		if (_usingKeys[i] == key) {
			int countLeftKeys = _countObjects - i;
			memmove(_usingKeys + i, _usingKeys + i + 1, sizeof(UINT) * countLeftKeys);
		}
	}
	--_countObjects;
}

void ObjectManager::UpdateObject(GameObject* obj)
{
	if (obj == nullptr) {
		__debugbreak();
		return;
	}

	UINT key = obj->GetID();
	if (key > _keyMax) {
		__debugbreak();
		return;
	}
	BOOL res = _objects[key].UpdateFrom(obj); // 
}

GameObject* ObjectManager::GetObjectPtrOrNull(UINT key)
{
	if (key > _keyMax) {
		__debugbreak();
		return NULL;
	}

	for (int i = 0; i < _countObjects; ++i) {
		if (_usingKeys[i] == key) {
			return _objects + key;
		}
	}

	return NULL;
}

void ObjectManager::GetTransformedModelOf(UINT key, Vertex* out_vertices, UINT* out_numVectors)
{
	if (key > _keyMax) {
		__debugbreak();
		return;
	}
	*out_numVectors = _objects[key].GetTransformedModel(out_vertices);
}

void ObjectManager::GetKeys(UINT* out_keys) const
{
	if (out_keys == nullptr) {
		__debugbreak();
		return;
	}

	for (int i = 0; i < _countObjects; ++i) {
		out_keys[i] = _usingKeys[i];
	}
}

int ObjectManager::GetCountObjects() const
{
	return _countObjects;
}
