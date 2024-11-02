#pragma once

#include "GameObject.h"



class Tank: public GameObject
{
public:
	Tank(UINT16 id, UINT32 ownerId);
	Tank() = default;
	~Tank();

	void Initiate(UINT16 id);
	void Terminate();

	void StartMove(EMOVEMENT movement);
	void EndMove(EMOVEMENT movement);

	void StartRotate(EROTATION rotation);
	void EndRotate(EROTATION rotation);

	void GetTurretInfo(Vector3* out_position, Vector3* out_direction) const;

	void OnFrame(ULONGLONG tickDiff) override;

	virtual void OnHit(ULONGLONG currentTick) override;

protected:
	virtual BOOL IsTransformCloseEnough(const Transform* other) override;

private:
	Vector3 _forwardDirection;
	
	bool _isMovingFoward = false;
	bool _isMovingBackward = false;
	bool _isRotatingLeft = false;
	bool _isRotatingRight = false;

	void MoveForward(ULONGLONG tickDiff);
	void MoveBackward(ULONGLONG tickDiff);
	void RotateRight(ULONGLONG tickDiff);
	void RotateLeft(ULONGLONG tickDiff);

};