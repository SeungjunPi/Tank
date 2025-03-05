#pragma once

#include "GameStruct.h"
#include "SentinelAliases.h"
#include "StableFlowStaticData.h"

const Vector3 FORWARD_DIRECTION = { .0f, -1.0f, .0f };
const Vector3 BACKWARD_DIRECTION = { .0f, 1.0f, .0f };

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
	UserDBIndex GetOwnerId() const;
	const Transform* GetTransformPtr() const;
	Vector3 GetPosition() const;

	void ResetDynamicPhysicalComponent();

	// other object
	BOOL UpdateTransformIfValid(const Transform* pTransform);


	virtual void Tick(ULONGLONG tickDiff);

	virtual BOOL IsAlive() const { return _isAlive; }

	virtual void Respawn();

	virtual BOOL IsDestroyed(ULONGLONG currentTick) const;

	void AttachCollider(Collider* pCollider);
	BOOL IsCollidable() { return _pCollider != nullptr; }

	Collider* GetColliderPtr() const { return _pCollider; }
	PhysicalComponent* GetPhysicalComponentPtr() { return &_physicalComponent; }
	ULONGLONG GetHitTick() const { return _hitTick; }
	void ApplyStableFlowResult();



	
	virtual void OnUpdateTransform() = 0;
	virtual void OnHitWith(ULONGLONG currentTick, GameObject* other) = 0;
protected:
	BOOL _isActivatable = false; // TODO: 탱크 외에도 이런게 없으면 탱크 처리 로직을 아예 분리하는 편이 나으므로, 고려해보기.

	ObjectID _id = INVALID_OBJECT_ID;
	UserDBIndex _ownerIndex = 0;
	BOOL _isAlive = true;


	// Physics
	PhysicalComponent _physicalComponent;
	Collider* _pCollider = nullptr;

	ULONGLONG _hitTick = 0;

	// 기본적으로 완전히 일치해야 같다고 판정
	virtual BOOL IsTransformCloseEnough(const Transform* other);
private:

};


