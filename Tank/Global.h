#pragma once

#include "stdafx.h"

class NetCore;
class Tank;
class AllocObjectManager;


const ULONGLONG GAME_FPS = 60;
const ULONGLONG TICK_PER_GAME_FRAME = 1000 / GAME_FPS;

extern ULONGLONG g_previousGameTick;
extern ULONGLONG g_currentGameTick;
extern DWORD g_currentFPS;

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

