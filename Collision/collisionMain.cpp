

#include "ICollisionManager.h"
#include "Collider.h"
#include "GameMath.h"

int main()
{
	ICollisionManager* pCollisionManager;
	CreateCollisionManager(&pCollisionManager);

	Collider* pC1;
	Collider* pC2;
	Vector3 v1{ 1.0f, 1.0f, 0.0f };
	Vector3 v1Velocity{ 0.f, 0.f, 0.f };
	Vector3 v2{ 1.0f, 1.0f, 0.0f };
	Vector3 v2Velocity{ 0.f, 0.f, 0.f };

	pC1 = pCollisionManager->GetNewColliderPtr(2.0, nullptr, &v1, &v1Velocity, 1.0f, 0b1);
	pC2 = pCollisionManager->GetNewColliderPtr(1.0, nullptr, &v2, &v2Velocity, 2.0f, 0b10);

	pCollisionManager->DetectCollision();

	

	
	printf("Hello, Collision\n");

	//float radius, GameObject* pObj, const Vector3* center, const Vector3* velocity, float mass, UINT32 kindness

	


}