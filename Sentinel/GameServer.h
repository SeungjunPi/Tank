#pragma once

#include "SentinelPch.h"

class GameServer
{
public:
	static void Initialize();
	static void Start();

	static void ActivateEndFlag();

	static void End();
	static void CleanUp();

	static void Broadcast(BYTE* msg, int len);
	static void BroadcastExcept(BYTE* msg, int len, UINT32 sessionId);
};

