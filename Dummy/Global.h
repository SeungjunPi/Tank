#pragma once

#include "TankPch.h"

class NetCore;
class AllocObjectManager;
class ICollisionManager;
class GameCamera;
class Player;
class DummyManager;

struct Score
{
	int kill;
	int death;
	int hit;
};


const ULONGLONG GAME_FPS = 60;
const ULONGLONG TICK_PER_GAME_FRAME = 1000 / GAME_FPS; // 

const ULONGLONG TICK_OWN_TANK_SYNC = TICK_PER_GAME_FRAME * 30; // once at 10 frames

extern ULONGLONG g_previousGameTick;
extern ULONGLONG g_currentGameTick;
extern DWORD g_currentFPS;

extern ULONGLONG g_lastOwnTankSyncTick;

extern NetCore* g_pNetCore;

extern bool g_isTankCreateRequest;

extern Score g_score;
extern AllocObjectManager g_objectManager;

extern std::wstring g_userName;
extern std::wstring g_password;

extern ICollisionManager* g_pCollisionManager;

extern GameCamera* g_pCamera;

extern DummyManager* g_pDummyManager;

class Global
{
public:
	static void Initiate();
	static void Terminate();

private:
	
};

