#define _CRTDBG_MAP_ALLOC

#include <string>
#include <stdlib.h>
#include <crtdbg.h>

#include <cstdio>

#include "Global.h"
#include "DummyGame.h"
#include <atlconv.h>



int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Global::Initiate();

	DummyGame::Initialize();

	DummyGame::Start();
	DummyGame::CleanUp();


	Global::Terminate();

	return 0;
}


