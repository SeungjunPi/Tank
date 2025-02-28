#pragma once


#include "GameStruct.h"
#include "TankAliases.h"
#include "StableFlowStaticData.h"


const Vector3 FORWARD_DIRECTION = { .0f, -1.0f, .0f };
const Vector3 BACKWARD_DIRECTION = { .0f, 1.0f, .0f };
 

class Collider;


class GameObject
{
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
	
	PhysicalComponent* GetPhysicalComponentPtr() { return &_physicalComponent; }
	ULONGLONG GetHitTick() const { return _hitTick; }
	void ApplyStableFlowResult(ULONGLONG tickDiff);


	virtual void OnHitWith(ULONGLONG currentTick, GameObject* other) = 0;
	virtual void OnUpdateTransform() = 0;

	virtual void Tick(ULONGLONG tickDiff);

	virtual BOOL IsAlive() const { return _isAlive; }

	virtual BOOL IsDestroyed(ULONGLONG currentTick) const;

	virtual void OnRespawn();

	virtual void OnHitServer(ULONGLONG currentTick, GameObject* other) = 0;
	
protected:
	BOOL _isActivatable = false; // TODO: 탱크 외에도 이런게 없으면 탱크 처리 로직을 아예 분리하는 편이 나으므로, 고려해보기.
	ObjectID _id = INVALID_OBJECT_ID;
	UserDBIndex _ownerID = 0;
	BOOL _isAlive = true;


	// Physics
	PhysicalComponent _physicalComponent;
	Collider* _pCollider = nullptr;
	ULONGLONG _hitTick = 0;

	// Rendering
	Model _model;
private:
	
};





