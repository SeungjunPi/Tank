#pragma once

#include "stdafx.h"

class NetCore;
class Tank;
class AllocObjectManager;


const ULONGLONG GAME_FPS = 60;
const ULONGLONG TICK_PER_GAME_FRAME = 1000 / GAME_FPS; // 

const ULONGLONG TICK_OWN_TANK_SYNC = TICK_PER_GAME_FRAME * 10; // once at 10 frames

extern ULONGLONG g_previousGameTick;
extern ULONGLONG g_currentGameTick;
extern DWORD g_currentFPS;

extern ULONGLONG g_lastOwnTankSyncTick;

extern NetCore* g_pNetCore;
extern UINT32 g_serverId;

extern bool g_isTankCreateRequest;
extern Tank* g_pPlayerTank;
extern AllocObjectManager g_objectManager;

extern UINT32 g_playerId;

class Global
{
public:
	static void Initiate();
	static void Terminate();

private:
	
};

