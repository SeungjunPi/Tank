#pragma once

#include "TankPch.h"

class INetCore;
class AllocObjectManager;
class IStableFlow;
class GameCamera;
class Player;

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

extern bool g_isTankCreateRequest;
extern Score g_score;
extern AllocObjectManager g_objectManager;

extern std::wstring g_userName;
extern std::wstring g_password;

extern IStableFlow* g_pStableFlow;

extern GameCamera* g_pCamera;

extern UINT64 g_keyboardPressedFlag;
extern UINT64 g_keyboardReleasedFlag;
extern UINT64 g_keyboardHeldFlag;


extern Player* g_pPlayer;

class Global
{
public:
	static void Initiate();
	static void Terminate();

private:
	
};

