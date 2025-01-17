#include "Global.h"

#include "NetCore.h"
#include "JunDB.h"

#include "PlayerManager.h"
#include "ObjectManager.h"


#include "ICollisionManager.h"

NetCore* g_pNetCore = nullptr;
UINT32* g_sessionIds = nullptr;
ULONGLONG g_previousGameTick = 0;
ULONGLONG g_currentGameTick = 0;

IJunDB* g_pJunDB = nullptr;
ICollisionManager* g_pCollisionManager = nullptr;

PlayerManager g_playerManager;
ObjectManager g_objectManager;