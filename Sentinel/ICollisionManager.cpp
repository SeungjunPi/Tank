#include "ICollisionManager.h"
#include "CollisionManager.h"

void CreateCollisionManager(ICollisionManager** dst)
{
	*dst = DNew CollisionManager;
}

void DeleteCollisionManager(ICollisionManager* ptr)
{
	delete ptr;
}
