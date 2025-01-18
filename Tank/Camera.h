#pragma once

#include "TankPch.h"
#include "TankAliases.h"
#include "GameMath.h"

class GameCamera
{
public:
	GameCamera();
	~GameCamera() = default;

	Vector3 GetPosition() const;

	void UpdateTransf(const Transform* to);
	
private:
	Transform _transf;
};