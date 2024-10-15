#pragma once


#include "stdafx.h"

class NetCore;

const ULONGLONG GAME_FPS = 60;
const ULONGLONG TICK_PER_GAME_FRAME = 1000 / GAME_FPS;

extern NetCore* g_pNetCore;
extern UINT32* g_sessionIds;

extern ULONGLONG g_previousGameTick;


