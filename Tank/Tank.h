#pragma once

#include "GameObject.h"


class Collider;

class Tank : public GameObject
{
public:
	static const float COLLIDER_RADIUS;
	static const ULONGLONG MACHINE_GUN_DELAY;
	static const float COLLIDER_MASS;

	Tank(ObjectID id, UserDBIndex ownerID);
	Tank() = default;
	~Tank();

	void Initiate(ObjectID id);
	void Terminate();

	void SetMovementState(bool forward, bool backward, bool left, bool right);



	void StartMove(EMovement movement);
	void EndMove(EMovement movement);
	void EndMove(EMovement movement, const Transform* pTransform);

	void StartRotate(ERotation rotation);
	void EndRotate(ERotation rotation);
	void EndRotate(ERotation rotation, const Transform* pTransform);

	void MoveForward(ULONGLONG tickDiff);
	void MoveBackward(ULONGLONG tickDiff);
	void RotateRight(ULONGLONG tickDiff);
	void RotateLeft(ULONGLONG tickDiff);

	void GetTurretInfo(Vector3* out_position, Vector3* out_direction) const;
	void GetTurretInfo(Transform* out_transform) const;

	void SetMachineGunFiringFlag() { _flagFiringMachineGun = true; }
	void ClearMachineGunFiringFlag() { _flagFiringMachineGun = false; }
	bool CanFireMachineGun() const;
	void OnFiringMachineGun(ULONGLONG currentTick);

	void ResetHP();

	virtual BOOL IsDestroyed(ULONGLONG currentTick) const override;

	virtual void OnFrame(ULONGLONG tickDiff) override;
	virtual void OnHit(ULONGLONG currentTick) override;
	virtual void OnUpdateTransform() override;
	virtual void OnRespawn() override;

	void OnHitByProjectile(ULONGLONG currentTick);
	

private:
	UserDBIndex _ownerId = 0;

	Transform _prevTransform = { 0, };

	int _hp = 0;

	bool _isMovingFoward = false;
	bool _isMovingBackward = false;
	bool _isRotatingLeft = false;
	bool _isRotatingRight = false;
	ULONGLONG _lastTransformSyncTick = 0;

	ULONGLONG _lastMachineGunFiringTick = 0;
	bool _flagFiringMachineGun = false;
	
	
};


