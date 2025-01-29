#pragma once

#include "GameObject.h"

class Collider;

class Projectile: public GameObject
{
public:
	static const float COLLIDER_RADIUS;
	static const float COLLIDER_MASS;
	static const float SPEED_PER_MS;
	Projectile() = default;
	~Projectile() = default;

	void Initiate(ObjectID id, Transform* transform, UserDBIndex ownerId);
	void Terminate();

	void OnFrame(ULONGLONG tickDiff) override;

	BOOL IsDestroyed(ULONGLONG currentTick) const override;

	virtual void OnHit(ULONGLONG currentTick) override;
	virtual void OnUpdateTransform() override;

private:
	Vector3 _forwardDirection = FORWARD_DIRECTION;
	ULONGLONG _genTick = 0;

	BOOL IsTimeout() const;
	void Move(ULONGLONG tickDiff);
};