#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>

#include <cstdio>

#include "Global.h"
#include "Game.h"



int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	Game::Initialize();
	
	Game::Start();
	Game::CleanUp();
	
	return 0;
}


