#pragma once

#include "GameObject.h"



class Tank : public GameObject
{
public:
	const static UINT OBJ_KEY = 0x10000;

	Tank(UINT32 id);
	Tank() = default;
	~Tank();

	void Initiate(UINT32 id);
	void Terminate();

	void MoveForward(ULONGLONG tickDiff);
	void MoveBackward(ULONGLONG tickDiff);
	void RotateRight(ULONGLONG tickDiff);
	void RotateLeft(ULONGLONG tickDiff);

	void GetTurretInfo(Vector3* out_position, Vector3* out_direction) const;
	void GetTurretInfo(Transform* out_transform) const;

	void OnFrame(ULONGLONG tickDiff) override;
private:
	Vector3 _forwardDirection;
	UINT32 _ownerId = 0;
};


