#pragma once

#include "GameObject.h"

class Collider;

class Tank: public GameObject
{
public:
	static const float COLLIDER_RADIUS;

	Tank(ObjectID id, UserDBIndex ownerId, Collider* pCollider);
	Tank() = default;
	~Tank();

	void Initiate(ObjectID id);
	void Terminate();

	void StartMove(EMOVEMENT movement);
	void EndMove(EMOVEMENT movement);

	void StartRotate(EROTATION rotation);
	void EndRotate(EROTATION rotation);

	void GetTurretInfo(Vector3* out_position, Vector3* out_direction) const;

	void OnFrame(ULONGLONG tickDiff) override;

	Collider* GetCollider() { return _pCollider; }

	virtual void OnHit(ULONGLONG currentTick) override;
	virtual void OnUpdateTransform() override;

	virtual void Respawn() override;

	

protected:
	virtual BOOL IsTransformCloseEnough(const Transform* other) override;

private:
	Vector3 _forwardDirection;
	
	bool _isMovingFoward = false;
	bool _isMovingBackward = false;
	bool _isRotatingLeft = false;
	bool _isRotatingRight = false;

	Collider* _pCollider = nullptr;

	Transform _prevTransform = { 0, };

	void MoveForward(ULONGLONG tickDiff);
	void MoveBackward(ULONGLONG tickDiff);
	void RotateRight(ULONGLONG tickDiff);
	void RotateLeft(ULONGLONG tickDiff);

	

};