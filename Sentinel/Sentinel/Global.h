#pragma once


#include "stdafx.h"

class NetCore;
class IJunDB;

const ULONGLONG GAME_FPS = 60;
const ULONGLONG TICK_PER_GAME_FRAME = 1000 / GAME_FPS;
const ULONGLONG TICK_TANK_RESPAWN_INTERVAL = 5000;

extern NetCore* g_pNetCore;
extern UINT32* g_sessionIds;

extern ULONGLONG g_currentGameTick;
extern ULONGLONG g_previousGameTick;

extern IJunDB* g_pJunDB;


