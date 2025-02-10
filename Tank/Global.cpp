

#include "Global.h"

#include "INetCore.h"
#include "Tank.h"
#include "AllocObjectManager.h"
#include "ICollisionManager.h"
#include "Physics.h"


ULONGLONG g_previousGameTick = 0;
ULONGLONG g_currentGameTick = 0;
ULONGLONG g_gameTickDiff = 0;
DWORD g_currentFPS = 0;

INetCore* g_pNetCore;
UINT32 g_serverId;

bool g_isTankCreateRequest = false;
Player* g_pPlayer = nullptr;
Score g_score;
AllocObjectManager g_objectManager;

UINT32 g_playerId;

std::wstring g_userName;
std::wstring g_password;

ICollisionManager* g_pCollisionManager;

Physics* g_pPhysics;

GameCamera* g_pCamera;

void Global::Initiate()
{
}

void Global::Terminate()
{
}
