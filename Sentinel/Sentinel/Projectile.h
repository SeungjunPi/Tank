#pragma once


#include "GameObject.h"

class Collider;

class Projectile: public GameObject
{
public:
	static const float COLLIDER_RADIUS;
	Projectile() = default;
	~Projectile() = default;

	void Initiate(ObjectID id, Transform* transform, UserDBIndex ownerId, Collider* pCollider);
	void Terminate();

	void OnFrame(ULONGLONG tickDiff) override;

	Collider* GetCollider() { return _pCollider; }

	virtual void OnHit(ULONGLONG currentTick) override;
	virtual void OnUpdateTransform() override;

private:
	Vector3 _forwardDirection = FORWARD_DIRECTION;
	ULONGLONG _genTick = 0;

	Collider* _pCollider = nullptr;

	BOOL IsTimeout() const;
	void Move(ULONGLONG tickDiff);
};