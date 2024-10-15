#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>

#include <cstdio>

#include "Global.h"
#include "Game.h"

int main()
{

	Game::Initialize();

	Game::Start();



	Game::CleanUp();

	_CrtDumpMemoryLeaks();
	
	return 0;
}


