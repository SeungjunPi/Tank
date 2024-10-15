#pragma once

#include "GameObject.h"
#include "GameStruct.h"


class DummyObject : public GameObject
{
public:
	DummyObject(UINT16 id): GameObject(id) {}


private:
	Transform _transform;
	Vector3 _moveDirection;
};