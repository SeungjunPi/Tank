#pragma once


#include "GameObject.h"

class Projectile : public GameObject
{
public:
	Projectile() = default;
	~Projectile() = default;

	void Initiate(ObjectID id, Transform* transform, UserDBIndex ownerID);
	void Terminate();

	void Tick(ULONGLONG tickDiff) override;
	BOOL IsDestroyed(ULONGLONG currentTick) const override;

	virtual void PreProcessMovementState() override;
	virtual void OnHitWith(ULONGLONG currentTick, GameObject* other) override;

	virtual void OnHitServer(ULONGLONG currentTick, GameObject* other) override;
	virtual void OnUpdateTransform() override;

private:
	ULONGLONG _genTick = 0;

	BOOL IsTimeout() const;
};