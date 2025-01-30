#pragma once


#include "GameObject.h"

class Obstacle : public GameObject
{
public:
	const static UINT DESTROY_DELAY = 0x1000;
	Obstacle() = default;
	~Obstacle() = default;

	void Initiate(ObjectID id, Transform* pInitTransform);
	void Terminate();

	void OnHitWith(ULONGLONG currentTick, GameObject* pOther) override;
	BOOL IsDestroyed(ULONGLONG currentTick) const override;

private:
	ULONGLONG _hitTick = 0;


};