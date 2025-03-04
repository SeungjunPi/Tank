#pragma once

#include "GameObject.h"
#include "GameStruct.h"



class Collider;

class Tank: public GameObject
{
public:
	Tank(ObjectID id, UserDBIndex ownerId);
	Tank() = default;
	~Tank();

	void GetTurretInfo(Transform* out_position, Vector3* out_direction) const;

	void ResetHP();

	void AdvancePlayerInput(PlayerInputState inputState);

	virtual void Tick(ULONGLONG tickDiff) override;

	virtual BOOL IsDestroyed(ULONGLONG currentTick) const;

	virtual void OnHitWith(ULONGLONG currentTick, GameObject* other) override;
	virtual void OnUpdateTransform() override;
	virtual void Respawn() override;

protected:
	virtual BOOL IsTransformCloseEnough(const Transform* other) override;

private:
	ULONGLONG _lastMachineGunFiringTick = 0;

	int _hp = 0;

	BOOL TryFireMachineGun(ULONGLONG currentTick);
};