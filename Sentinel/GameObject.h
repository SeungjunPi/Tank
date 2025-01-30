#pragma once

#include "GameStruct.h"
#include "SentinelAliases.h"

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

class Collider;

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

	// other object
	BOOL UpdateFrom(const GameObject* pOther);
	void UpdateTransform(const Transform* pTransform);
	BOOL UpdateTransformIfValid(const Transform* pTransform);

	void Move();

	void SetPosition(Vector3 position);

	BOOL IsDirty();

	UserDBIndex GetOwnerId() const;

	virtual void OnFrame(ULONGLONG tickDiff) = 0;

	virtual BOOL IsAlive() const { return _isAlive; }

	virtual void OnRespawn();

	virtual BOOL IsDestroyed(ULONGLONG currentTick) const;

	virtual void OnUpdateTransform() {}

	void AttachCollider(Collider* pCollider);
	BOOL IsCollidable() { return _pCollider != nullptr; }

	Collider* GetColliderPtr() const { return _pCollider; }
	ULONGLONG GetHitTick() const { return _hitTick; }

	virtual Vector3 GetMovementVector();
	
	void UpdateTransform();

	virtual void OnHit(ULONGLONG currentTick) {}
protected:
	BOOL _isActivatable = false; // TODO: 탱크 외에도 이런게 없으면 탱크 처리 로직을 아예 분리하는 편이 나으므로, 고려해보기.
	Transform _transform = { 0, };
	ObjectID _id = INVALID_OBJECT_ID;
	UserDBIndex _ownerIndex = 0;
	BOOL _dirty = false;
	BOOL _isAlive = true;

	
	// Physics
	Vector3 _movementDirection = FORWARD_DIRECTION;
	float _speed = 0.f; // 움직이지 않기 위해 이렇게 설정 
	float _mass = 1000000.f;// 움직이지 않기 위해 이렇게 설정
	
	Collider* _pCollider = nullptr;
	ULONGLONG _hitTick = 0;

	const static UINT DESTROY_DELAY = 0x1000;

	// 기본적으로 완전히 일치해야 같다고 판정
	virtual BOOL IsTransformCloseEnough(const Transform* other);
private:

};


