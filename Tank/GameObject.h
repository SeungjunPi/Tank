#pragma once


#include "GameStruct.h"
#include "TankAliases.h"


const Vector3 FORWARD_DIRECTION = { .0f, -1.0f, .0f };

class Collider;
class Physics;

class GameObject
{
	friend Physics;
public:
	GameObject();
	GameObject(ObjectID id, UserDBIndex ownerID);
	GameObject(ObjectID id, UserDBIndex ownerID, BOOL activatable);
	~GameObject();

	ObjectID GetID() const;
	Transform GetTransform() const;
	UserDBIndex GetOwnerID() const;
	const Transform* GetTransformPtr() const;
	Vector3 GetPosition() const;

	UINT GetTransformedModel(Vertex* out_vertices);

	void SetTransformByServer(const Transform* pTransform);


	void Respawn();

	void AttachCollider(Collider* pCollider);
	BOOL IsCollidable() { return _pCollider != nullptr; }

	Collider* GetColliderPtr() { return _pCollider; }
	void SyncTransformWithCollider();
	ULONGLONG GetHitTick() const { return _hitTick; }
	void ApplyNextMovement(ULONGLONG tickDiff);

	virtual void PreProcessMovementState() = 0;
	virtual void OnHitWith(ULONGLONG currentTick, GameObject* other) = 0;
	virtual void OnUpdateTransform() = 0;

	virtual void Tick(ULONGLONG tickDiff);

	virtual BOOL IsAlive() const { return _isAlive; }

	virtual BOOL IsDestroyed(ULONGLONG currentTick) const;

	virtual void OnRespawn();

	virtual void OnHitServer(ULONGLONG currentTick, GameObject* other) = 0;
	
protected:
	BOOL _isActivatable = false; // TODO: 탱크 외에도 이런게 없으면 탱크 처리 로직을 아예 분리하는 편이 나으므로, 고려해보기.
	Transform _transform;
	ObjectID _id = INVALID_OBJECT_ID;
	UserDBIndex _ownerID = 0;
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

	// Rendering
	Model _model;
private:
	
};





