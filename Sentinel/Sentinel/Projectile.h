#pragma once


#include "GameObject.h"

class Projectile: public GameObject
{
public:

	Projectile() = default;
	~Projectile() = default;

	void Initiate(UINT32 id, Transform* transform);
	void Terminate();

	void OnFrame(ULONGLONG tickDiff) override;

private:
	Vector3 _forwardDirection = FORWARD_DIRECTION;
	ULONGLONG _genTick = 0;

	BOOL IsTimeout() const;
	void Move(ULONGLONG tickDiff);
};