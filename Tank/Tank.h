#pragma once

#include "GameObject.h"




class Tank : public GameObject
{
public:
	Tank(ObjectID id);
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

	void OnFrame(ULONGLONG tickDiff) override;

	virtual void OnHit(ULONGLONG currentTick) override;

private:
	Vector3 _forwardDirection;
	UserDBIndex _ownerId = 0;

	bool _isMovingFoward = false;
	bool _isMovingBackward = false;
	bool _isRotatingLeft = false;
	bool _isRotatingRight = false;
};


