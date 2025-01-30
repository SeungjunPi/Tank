#pragma once

#include "TankPch.h"

class NetCore;
class AllocObjectManager;
class ICollisionManager;
class GameCamera;
class Player;
class Physics;

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
extern ULONGLONG g_gameTickDiff;
extern DWORD g_currentFPS;

extern NetCore* g_pNetCore;

extern bool g_isTankCreateRequest;
extern Player* g_pPlayer;
extern Score g_score;
extern AllocObjectManager g_objectManager;

extern std::wstring g_userName;
extern std::wstring g_password;

extern ICollisionManager* g_pCollisionManager;
extern Physics* g_pPhysics;

extern GameCamera* g_pCamera;

class Global
{
public:
	static void Initiate();
	static void Terminate();

private:
	
};

