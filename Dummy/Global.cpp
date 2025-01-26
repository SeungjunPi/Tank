

#include "Global.h"

#include "NetCore.h"
#include "Tank.h"
#include "AllocObjectManager.h"
#include "ICollisionManager.h"
#include "DummyManager.h"


ULONGLONG g_previousGameTick = 0;
ULONGLONG g_currentGameTick = 0;
DWORD g_currentFPS = 0;

NetCore* g_pNetCore;

AllocObjectManager g_objectManager;

ICollisionManager* g_pCollisionManager;

GameCamera* g_pCamera;

DummyManager* g_pDummyManager;

void Global::Initiate()
{
}

void Global::Terminate()
{
}
