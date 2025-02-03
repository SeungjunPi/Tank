#pragma once

#include "GameStruct.h"
#include "SentinelAliases.h"

const Vector3 FORWARD_DIRECTION = { .0f, -1.0f, .0f };

class Collider;
class Physics;

class GameObject
{
	friend Physics;
public:
	GameObject();
	GameObject(ObjectID id, UserDBIndex ownerId);
	GameObject(ObjectID id, UserDBIndex ownerId, BOOL activatable);
	~GameObject();

	ObjectID GetID() const;
	Transform GetTransform() const;
	UserDBIndex GetOwnerId() const;
	const Transform* GetTransformPtr() const;
	Vector3 GetPosition() const;

	// other object
	BOOL UpdateTransformIfValid(const Transform* pTransform);


	virtual void Tick(ULONGLONG tickDiff);

	virtual BOOL IsAlive() const { return _isAlive; }

	virtual void Respawn();

	virtual BOOL IsDestroyed(ULONGLONG currentTick) const;

	void AttachCollider(Collider* pCollider);
	BOOL IsCollidable() { return _pCollider != nullptr; }

	Collider* GetColliderPtr() const { return _pCollider; }
	void SyncTransformWithCollider();
	ULONGLONG GetHitTick() const { return _hitTick; }
	void ApplyNextMovement(ULONGLONG tickDiff);

	virtual void PreProcessMovementState() = 0;
	virtual void OnUpdateTransform() = 0;
	virtual void OnHitWith(ULONGLONG currentTick, GameObject* other) = 0;
protected:
	BOOL _isActivatable = false; // TODO: 탱크 외에도 이런게 없으면 탱크 처리 로직을 아예 분리하는 편이 나으므로, 고려해보기.
	Transform _transform = { 0, };
	ObjectID _id = INVALID_OBJECT_ID;
	UserDBIndex _ownerIndex = 0;
	BOOL _isAlive = true;


	// Physics
	Vector3 _translationDirection = FORWARD_DIRECTION;
	float _translationSpeed = 0.f;
	float _rotationAngle = 0.f; // Z축을 기준으로만 고려함
	float _mass = 1000000.f; // 기본적으로 움직이지 않기 위해 이렇게 설정	
	float _radius = 0.0f; // 크기 없음

	Vector3 _nextPosition;
	float _nextRotationAngle = 0.0f;

	Collider* _pCollider = nullptr;
	ULONGLONG _hitTick = 0;

	// 기본적으로 완전히 일치해야 같다고 판정
	virtual BOOL IsTransformCloseEnough(const Transform* other);
private:

};


