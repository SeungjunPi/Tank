#pragma once

#include "GameObject.h"


class Collider;

class Tank : public GameObject
{
public:
	static const float COLLIDER_RADIUS;
	static const float MACHINE_GUN_DELAY;

	Tank(ObjectID id, UserDBIndex ownerID);
	Tank() = default;
	~Tank();

	void Initiate(ObjectID id);
	void Terminate();

	void StartMove(EMOVEMENT movement);
	void EndMove(EMOVEMENT movement);
	void EndMove(EMOVEMENT movement, const Transform* pTransform);

	void StartRotate(EROTATION rotation);
	void EndRotate(EROTATION rotation);
	void EndRotate(EROTATION rotation, const Transform* pTransform);

	void MoveForward(ULONGLONG tickDiff);
	void MoveBackward(ULONGLONG tickDiff);
	void RotateRight(ULONGLONG tickDiff);
	void RotateLeft(ULONGLONG tickDiff);

	void GetTurretInfo(Vector3* out_position, Vector3* out_direction) const;
	void GetTurretInfo(Transform* out_transform) const;

	void SetMachineGunFiringFlag() { _flagFiringMachineGun = true; }
	void ClearMachineGunFiringFlag() { _flagFiringMachineGun = false; }
	void OnFiringMachineGun(ULONGLONG currentTick);

	void ResetHP();

	virtual BOOL IsDestroyed(ULONGLONG currentTick) const override;

	virtual void OnFrame(ULONGLONG tickDiff) override;
	virtual void OnHit(ULONGLONG currentTick) override;
	virtual void OnUpdateTransform() override;
	virtual void OnRespawn() override;

	void OnHitByProjectile(ULONGLONG currentTick);
	

private:
	Vector3 _forwardDirection;
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


