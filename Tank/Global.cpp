

#include "Global.h"

#include "NetCore.h"
#include "Tank.h"
#include "AllocObjectManager.h"
#include "ICollisionManager.h"


ULONGLONG g_previousGameTick = 0;
ULONGLONG g_currentGameTick = 0;
DWORD g_currentFPS = 0;

ULONGLONG g_lastOwnTankSyncTick = 0;

NetCore* g_pNetCore;
UINT32 g_serverId;

bool g_isTankCreateRequest = false;
Tank* g_pPlayerTank;
AllocObjectManager g_objectManager;

UINT32 g_playerId;

Score g_score = { 0, };

std::wstring g_userName;
std::wstring g_password;

ICollisionManager* g_pCollisionManager;

void Global::Initiate()
{
}

void Global::Terminate()
{
}
