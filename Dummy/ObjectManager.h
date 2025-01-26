#pragma once

#include "GameObject.h"


class ObjectManager
{
public:
	void Initiate(UINT keyMax);
	void Terminate();

	void AddObject(GameObject* obj);
	void RemoveObject(GameObject* obj);
	void RemoveObject(UINT key);

	void UpdateObject(GameObject* obj);
	GameObject* GetObjectPtrOrNull(UINT key);

	void GetTransformedModelOf(UINT key, Vertex* out_vertices, UINT* out_numVectors);

	void GetKeys(UINT* out_keys) const;

	int GetCountObjects() const;

private:
	GameObject* _objects = nullptr;
	UINT* _usingKeys = nullptr;
	int _countObjects = 0;
	UINT _keyMax = 0;
};


