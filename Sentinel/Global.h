#pragma once


#include "SentinelPch.h"

class INetCore;
class IJunDB;
class PlayerManager;
class ObjectManager;
class IStableFlow;

const ULONGLONG GAME_FPS = 60;
const ULONGLONG TICK_PER_GAME_FRAME = 1000 / GAME_FPS;
const ULONGLONG TICK_TANK_SYNC = TICK_PER_GAME_FRAME * 6;
const ULONGLONG TICK_TANK_RESPAWN_INTERVAL = 5000;

extern INetCore* g_pNetCore;
extern UINT32* g_sessionIds;

extern ULONGLONG g_currentGameTick;
extern ULONGLONG g_previousGameTick;
extern ULONGLONG g_diffGameTick;

extern IJunDB* g_pJunDB;
extern IStableFlow* g_pStableFlow;
extern PlayerManager g_playerManager;
extern ObjectManager g_objectManager;

