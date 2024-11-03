#include "Global.h"

#include "NetCore.h"

NetCore* g_pNetCore = nullptr;
UINT32* g_sessionIds = nullptr;
ULONGLONG g_previousGameTick = 0;
ULONGLONG g_currentGameTick = 0;