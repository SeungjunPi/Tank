#pragma once

#include "GameStruct.h"

const Vector3 FORWARD_DIRECTION = { .0f, -1.0f, .0f };

enum class EMOVEMENT
{
	NONE,
	FORWARD,
	BACKWARD
};

enum class EROTATION
{
	NONE,
	LEFT,
	RIGHT
};


class GameObject
{
public:
	GameObject();
	GameObject(UINT32 id);
	~GameObject();

	UINT32 GetID() const;
	Transform GetTransform() const;
	const Transform* GetTransformPtr() const;
	Vector3 GetPosition() const;

	UINT GetColliderSize() const;

	// other object
	BOOL UpdateFrom(const GameObject* pOther);
	void UpdateTransform(const Transform* pTransform);

	void SetPosition(Vector3 position);

	BOOL IsDirty();



	virtual void OnFrame(ULONGLONG tickDiff) { }

	virtual BOOL IsAlive() const { return _isAlive; }

	virtual void OnHit(ULONGLONG currentTick);
	virtual BOOL IsDestroyed(ULONGLONG currentTick) const;


protected:
	Transform _transform = { 0, };
	UINT32 _id = 0;
	float _colliderSize = 0;
	BOOL _dirty = false;
	BOOL _isAlive = true;
	ULONGLONG _hitTick = 0;

	const static UINT DESTROY_DELAY = 0x1000;
private:

};


