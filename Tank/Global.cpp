

#include "Global.h"

#include "INetCore.h"
#include "Tank.h"
#include "AllocObjectManager.h"
#include "IStableFlow.h"

ULONGLONG g_previousGameTick = 0;
ULONGLONG g_currentGameTick = 0;
ULONGLONG g_gameTickDiff = 0;
DWORD g_currentFPS = 0;

INetCore* g_pNetCore;
UINT32 g_serverId;

bool g_isTankCreateRequest = false;
Score g_score;
AllocObjectManager g_objectManager;

UINT32 g_playerId;

std::wstring g_userName;
std::wstring g_password;

IStableFlow* g_pStableFlow;

GameCamera* g_pCamera;

UINT64 g_keyboardPressedFlag = 0;
UINT64 g_keyboardReleasedFlag = 0;
UINT64 g_keyboardHeldFlag = 0;


Player* g_pPlayer = nullptr;

void Global::Initiate()
{
}

void Global::Terminate()
{
}
