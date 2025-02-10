#pragma once

#include "DummyPch.h"

class INetCore;
class AllocObjectManager;
class ICollisionManager;
class GameCamera;
class Player;
class DummyManager;
class Physics;

struct Score
{
	int kill;
	int death;
	int hit;
};


const ULONGLONG GAME_FPS = 60;
const ULONGLONG TICK_PER_GAME_FRAME = 1000 / GAME_FPS; // 

const ULONGLONG TICK_OWN_TANK_SYNC = TICK_PER_GAME_FRAME * 6; // once at 10 frames

extern ULONGLONG g_previousGameTick;
extern ULONGLONG g_currentGameTick;
extern ULONGLONG g_gameTickDiff;
extern DWORD g_currentFPS;

extern INetCore* g_pNetCore;

extern AllocObjectManager g_objectManager;
extern ICollisionManager* g_pCollisionManager;
extern Physics* g_pPhysics;

extern GameCamera* g_pCamera;
extern DummyManager* g_pDummyManager;

class Global
{
public:
	static void Initiate();
	static void Terminate();

private:
	
};

