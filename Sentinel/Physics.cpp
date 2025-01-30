
#include "Physics.h"

#include "ICollisionManager.h"
#include "Collider.h"
#include "GameObject.h"
#include "Global.h"
#include "ObjectManager.h"
#include "CommonData.h"

void Physics::Update()
{
	// detect collisions
	g_pCollisionManager->DetectCollision();

	// apply physics on collided objects (include resolve collision)
	JStack* collisionPairs = g_pCollisionManager->GetCollisionPairs();
	UINT32 size = collisionPairs->GetCount();
	for (UINT32 i = 0; i < size; ++i) {
		CollisionPair pair;
		collisionPairs->Top(&pair);
		collisionPairs->Pop();
		// physics on a, b
		CalculateElasticCollisionNextMovements(pair.a, pair.b);
		ResolvePenetration(pair.a, pair.b);

		pair.a->SyncTransformWithCollider();
		pair.b->SyncTransformWithCollider();
	}

	UINT32 keys[1024];
	UINT32 countKeys;

	int objectKindEnumMax = (int)GAME_OBJECT_TYPE_MAX;
	for (int i = 0; i < objectKindEnumMax; ++i) {
		EGameObjectType type = (EGameObjectType)i;
		g_objectManager.GetKeys(type, keys, &countKeys);
		for (UINT32 i = 0; i < countKeys; ++i) {
			GameObject* pObject = g_objectManager.GetObjectPtrOrNull(ObjectID{ type, (ObjectKey)keys[i] });
			if (pObject == NULL) {
				__debugbreak();
			}

			Collider* pCollider = pObject->GetColliderPtr();
			if (pCollider != nullptr && pCollider->IsCollided()) {
				continue;
			}
			
			pObject->UpdateTransform(g_diffGameTick);
			pObject->SyncTransformWithCollider();
		}
	}
}

void Physics::CalculateElasticCollisionNextMovements(GameObject* a, GameObject* b)
{
	Vector3 n = a->_transform.Position - b->_transform.Position;
	if (Vector3::Norm(n) < SAME_POSITION_THRESHOLD) {
		return;
	}
	Vector3::Normalize(&n, n);

	Vector3 v = a->_movementDirection * a->_movementSpeed;
	Vector3 w = b->_movementDirection * b->_movementSpeed;

	Vector3 v_n = n * Vector3::DotProduct(n, v);
	Vector3 v_t = v - v_n;

	Vector3 w_n = n * Vector3::DotProduct(n, w);
	Vector3 w_t = w - w_n;

	Vector3 v_np = (v_n * (a->_mass - b->_mass) + w_n * (2.0f * b->_mass)) * (1.0f / (a->_mass + b->_mass));
	Vector3 w_np = (w_n * (b->_mass - a->_mass) + v_n * (2.0f * a->_mass)) * (1.0f / (a->_mass + b->_mass));

	a->_transform.Position = v_np + v_t;
	b->_transform.Position = w_np + w_t;
}

void Physics::ResolvePenetration(GameObject* a, GameObject* b)
{
	Vector3 n = a->_transform.Position - b->_transform.Position;
	float distance = Vector3::Norm(n);
	if (distance < SAME_POSITION_THRESHOLD) {
		// 완전히 겹쳤을 시 
		n = { 1.0f, 0.f, 0.f };
	}
	float penetrationDepth = (a->_radius + b->_radius) - distance;

	if (penetrationDepth > 0.0f) {
		Vector3::Normalize(&n, n);

		float weight1 = b->_mass / (a->_mass + b->_mass);
		float weight2 = a->_mass / (a->_mass + b->_mass);

		a->_transform.Position = a->_transform.Position + n * (penetrationDepth * weight1);
		b->_transform.Position = b->_transform.Position - n * (penetrationDepth * weight2);
	}
}
