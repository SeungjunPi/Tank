
#include "Physics.h"

#include "ICollisionManager.h"
#include "Collider.h"
#include "GameObject.h"
#include "Global.h"
#include "ObjectManager.h"

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

		pair.a;
		pair.b;

		// physics on a, b
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
			
			// Rotate
			// Move
		}
	}
}
