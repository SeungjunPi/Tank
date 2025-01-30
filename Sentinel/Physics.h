#pragma once

#include "SentinelPch.h"
#include "GameObject.h"


class ICollisionManager;


class Physics
{
public:
	void Update();

private:
	void CalculateElasticCollisionNextMovements(GameObject* a, GameObject* b);
	void ResolvePenetration(GameObject* a, GameObject* b);
};
