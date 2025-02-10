

#include "Global.h"

#include "INetCore.h"
#include "Tank.h"
#include "AllocObjectManager.h"
#include "ICollisionManager.h"
#include "DummyManager.h"
#include "Physics.h"


ULONGLONG g_previousGameTick = 0;
ULONGLONG g_currentGameTick = 0;
ULONGLONG g_gameTickDiff = 0;
DWORD g_currentFPS = 0;

INetCore* g_pNetCore;

AllocObjectManager g_objectManager;

ICollisionManager* g_pCollisionManager;

GameCamera* g_pCamera;

DummyManager* g_pDummyManager;

Physics* g_pPhysics;

void Global::Initiate()
{
}

void Global::Terminate()
{
}
