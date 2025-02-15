﻿#pragma once

#include "GameObject.h"
#include "CommonData.h"

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
	void UpdatePlayerInputState(PlayerInputState inputState);

	virtual void PreProcessMovementState() override;
	virtual void Tick(ULONGLONG tickDiff) override;

	virtual BOOL IsDestroyed(ULONGLONG currentTick) const override;

	virtual void OnHitWith(ULONGLONG currentTick, GameObject* other) override;
	virtual void OnUpdateTransform() override;
	virtual void OnRespawn() override;

	virtual void OnHitServer(ULONGLONG currentTick, GameObject* other) override;
private:
	PlayerInputState _currentInputState = PLAYER_INPUT_NONE;
	PlayerInputState _previousInputState = PLAYER_INPUT_NONE;

	ULONGLONG _lastTransformSyncTick = 0;
	ULONGLONG _lastMachineGunFiringTick = 0;

	int _hp = 0;

	BOOL TryFireMachineGun(ULONGLONG currentTick);
	void ProcessInput();
};


