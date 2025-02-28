#include "Global.h"

#include "INetCore.h"
#include "JunDB.h"

#include "PlayerManager.h"
#include "ObjectManager.h"

#include "IStableFlow.h"

INetCore* g_pNetCore = nullptr;
UINT32* g_sessionIds = nullptr;
ULONGLONG g_diffGameTick = 0;
ULONGLONG g_previousGameTick = 0;
ULONGLONG g_currentGameTick = 0;

IJunDB* g_pJunDB = nullptr;
IStableFlow* g_pStableFlow = nullptr;

PlayerManager g_playerManager;
ObjectManager g_objectManager;
