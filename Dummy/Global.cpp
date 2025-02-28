

#include "Global.h"

#include "INetCore.h"
#include "Tank.h"
#include "AllocObjectManager.h"
#include "IStableFlow.h"
#include "DummyManager.h"



ULONGLONG g_previousGameTick = 0;
ULONGLONG g_currentGameTick = 0;
ULONGLONG g_gameTickDiff = 0;
DWORD g_currentFPS = 0;

INetCore* g_pNetCore;

AllocObjectManager g_objectManager;

IStableFlow* g_pStableFlow;

GameCamera* g_pCamera;

DummyManager* g_pDummyManager;

void Global::Initiate()
{
}

void Global::Terminate()
{
}
