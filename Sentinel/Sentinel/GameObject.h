#pragma once

#include "GameStruct.h"
#include "SentinelAliases.h"
#include "Collider.h"

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
	GameObject(ObjectID id, UserDBIndex ownerId);
	GameObject(ObjectID id, UserDBIndex ownerId, BOOL activatable);
	~GameObject();

	ObjectID GetID() const;
	Transform GetTransform() const;
	const Transform* GetTransformPtr() const;
	Vector3 GetPosition() const;

	UINT GetColliderSize() const;

	// other object
	BOOL UpdateFrom(const GameObject* pOther);
	void UpdateTransform(const Transform* pTransform);
	BOOL UpdateTransformIfValid(const Transform* pTransform);

	void SetPosition(Vector3 position);

	BOOL IsDirty();

	UserDBIndex GetOwnerId() const;



	virtual void OnFrame(ULONGLONG tickDiff) { }

	virtual BOOL IsAlive() const { return _isAlive; }

	virtual void Respawn();

	virtual void OnHit(ULONGLONG currentTick);
	virtual BOOL IsDestroyed(ULONGLONG currentTick) const;


protected:
	BOOL _isActivatable = false; // TODO: 탱크 외에도 이런게 없으면 탱크 처리 로직을 아예 분리하는 편이 나으므로, 고려해보기.
	Transform _transform = { 0, };
	ObjectID _id = 0;
	UserDBIndex _ownerIndex = 0;
	float _colliderSize = 0;
	BOOL _dirty = false;
	BOOL _isAlive = true;
	ULONGLONG _hitTick = 0;

	const static UINT DESTROY_DELAY = 0x1000;

	// 기본적으로 완전히 일치해야 같다고 판정
	virtual BOOL IsTransformCloseEnough(const Transform* other);
private:

};


