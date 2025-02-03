#pragma once


#include "DummyPch.h"
#include "GameObject.h"


class ICollisionManager;


class Physics
{
public:
	void ProcessNextMovement(ULONGLONG tickDiff);

private:
	void CalculateElasticCollisionNextMovements(GameObject* a, GameObject* b, ULONGLONG tickDiff);
	void ResolvePenetration(GameObject* a, GameObject* b);
};


