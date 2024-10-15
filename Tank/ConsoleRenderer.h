#pragma once

#include "GameStruct.h"
#include "GameObject.h"

#include "AllocObjectManager.h"

class ConsoleRenderer
{
public:
	static void Initiate(AllocObjectManager* pObjectManager); // Initialize Renderer.
	static void Terminate(); // Terminate Renderer.

	static void Render();

	static void TurnOnShowDebugInfo();
	static void TurnOffShowDebugInfo();
private:
	static void DrawObjects();
	static AllocObjectManager* _pAllocObjManager;
};

