#pragma once

#include "GameObject.h"
#include "GameStruct.h"

class Collider;

class Tank : public GameObject
{
public:
	Tank(ObjectID id, UserDBIndex ownerID);
	Tank() = default;
	~Tank();

	void GetTurretInfo(Transform* out_position, Vector3* out_direction) const;
	
	void ResetHP();

	void UpdatePlayerInputStateFromServer(PlayerInputState inputState);
	void AdvancePlayerInput(PlayerInputState inputState);

	virtual void Tick(ULONGLONG tickDiff) override;

	virtual BOOL IsDestroyed(ULONGLONG currentTick) const override;

	virtual void OnHitWith(ULONGLONG currentTick, GameObject* other) override;
	virtual void OnUpdateTransform() override;
	virtual void OnRespawn() override;

	virtual void OnHitServer(ULONGLONG currentTick, GameObject* other) override;

	BOOL TryFireMachineGun(ULONGLONG currentTick);
private:
	ULONGLONG _lastMachineGunFiringTick = 0;

	int _hp = 0;
};


