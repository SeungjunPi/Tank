#pragma once

#include "GameObject.h"

class Collider;

class Tank: public GameObject
{
public:
	Tank(ObjectID id, UserDBIndex ownerId);
	Tank() = default;
	~Tank();



	void StartMove(EMOVEMENT movement);
	void EndMove(EMOVEMENT movement);

	void StartRotate(EROTATION rotation);
	void EndRotate(EROTATION rotation);

	void GetTurretInfo(Vector3* out_position, Vector3* out_direction) const;

	void ResetHP();

	void OnFrame(ULONGLONG tickDiff) override;

	virtual Vector3 GetMovementVector(ULONGLONG tickDiff) override;
	virtual Quaternion GetDeltaRotation(ULONGLONG tickDiff) override;

	virtual BOOL IsDestroyed(ULONGLONG currentTick) const;

	virtual void OnHitWith(ULONGLONG currentTick, GameObject* other) override;

	virtual void OnRespawn() override;

	

protected:
	virtual BOOL IsTransformCloseEnough(const Transform* other) override;

private:
	bool _isMovingFoward = false;
	bool _isMovingBackward = false;
	bool _isRotatingLeft = false;
	bool _isRotatingRight = false;

	int _hp = 0;


	ULONGLONG _lastMachineGunFireTick = 0;
};