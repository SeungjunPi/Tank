#pragma once

#include "GameObject.h"
#include "CommonData.h"



class Collider;

class Tank: public GameObject
{
public:
	Tank(ObjectID id, UserDBIndex ownerId);
	Tank() = default;
	~Tank();

	void GetTurretInfo(Vector3* out_position, Vector3* out_direction) const;

	void ResetHP();

	void UpdatePlayerInputState(PlayerInputState inputState);

	virtual void PreProcessMovementState() override;
	virtual void Tick(ULONGLONG tickDiff) override;

	virtual BOOL IsDestroyed(ULONGLONG currentTick) const;

	virtual void OnHitWith(ULONGLONG currentTick, GameObject* other) override;
	virtual void OnUpdateTransform() override;
	virtual void Respawn() override;

protected:
	virtual BOOL IsTransformCloseEnough(const Transform* other) override;

private:
	PlayerInputState _crntInputState = PLAYER_INPUT_NONE;
	PlayerInputState _prevInputState = PLAYER_INPUT_NONE;

	ULONGLONG _lastTransformSyncTick = 0;
	ULONGLONG _lastMachineGunFiringTick = 0;
	

	int _hp = 0;

	BOOL TryFireMachineGun(ULONGLONG currentTick);
	void ProcessInput();
	
};