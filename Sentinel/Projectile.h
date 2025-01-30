#pragma once

#include "GameObject.h"

class Collider;

class Projectile: public GameObject
{
public:
	Projectile() = default;
	~Projectile() = default;

	void Initiate(ObjectID id, Transform* transform, UserDBIndex ownerId);
	void Terminate();

	void OnFrame(ULONGLONG tickDiff) override;

	BOOL IsDestroyed(ULONGLONG currentTick) const override;

	virtual void OnHitWith(ULONGLONG currentTick, GameObject* other) override;

private:
	ULONGLONG _genTick = 0;

	BOOL IsTimeout() const;
	void Move(ULONGLONG tickDiff);
};