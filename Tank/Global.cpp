

#include "Global.h"

#include "NetCore.h"
#include "Tank.h"
#include "AllocObjectManager.h"
#include "ICollisionManager.h"


ULONGLONG g_previousGameTick = 0;
ULONGLONG g_currentGameTick = 0;
DWORD g_currentFPS = 0;

NetCore* g_pNetCore;
UINT32 g_serverId;

bool g_isTankCreateRequest = false;
Player* g_pPlayer = nullptr;
Score g_score;
AllocObjectManager g_objectManager;

UINT32 g_playerId;

std::wstring g_userName;
std::wstring g_password;

ICollisionManager* g_pCollisionManager;

GameCamera* g_pCamera;

void Global::Initiate()
{
}

void Global::Terminate()
{
}
