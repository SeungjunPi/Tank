#pragma once

#include "PlayerManager.h"
#include "ObjectManager.h"

extern PlayerManager g_playerManager;
extern ObjectManager g_objectManager;

class GameServer
{
public:
	static void Initialize();
	static void Start();

	static void ActivateEndFlag();

	static void End();
	static void CleanUp();

	static void Broadcast(BYTE* msg, int len);
};

