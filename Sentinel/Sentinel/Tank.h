#pragma once

#include "GameObject.h"



class Tank: public GameObject
{
public:
	const static UINT OBJ_KEY = 0x10000;

	Tank(UINT32 id, UINT32 ownerId);
	Tank() = default;
	~Tank();

	void Initiate(UINT32 id);
	void Terminate();

	void StartMove(EMOVEMENT movement);
	void EndMove(EMOVEMENT movement);

	void StartRotate(EROTATION rotation);
	void EndRotate(EROTATION rotation);

	void GetTurretInfo(Vector3* out_position, Vector3* out_direction) const;

	UINT32 GetOwnerId() const;

	void OnFrame(ULONGLONG tickDiff) override;

protected:
	virtual BOOL IsTransformCloseEnough(const Transform* other) override;

private:
	Vector3 _forwardDirection;
	UINT32 _ownerId = 0;
	
	bool _isMovingFoward = false;
	bool _isMovingBackward = false;
	bool _isRotatingLeft = false;
	bool _isRotatingRight = false;

	void MoveForward(ULONGLONG tickDiff);
	void MoveBackward(ULONGLONG tickDiff);
	void RotateRight(ULONGLONG tickDiff);
	void RotateLeft(ULONGLONG tickDiff);

};