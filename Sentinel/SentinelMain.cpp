
#include <stdlib.h>
#include <crtdbg.h>


#include "SentinelPch.h"
#include "GameServer.h"
#include "NetCore.h"
#include "NetMessageQueue.h"


#include <signal.h>

void SignalHandler(int signal)
{
	if (signal == SIGINT) {
		GameServer::ActivateEndFlag();
	}
}


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	signal(SIGINT, SignalHandler);


	

	GameServer::Initialize();
	GameServer::Start();

	GameServer::End();

	GameServer::CleanUp();


	//_CrtDumpMemoryLeaks();


	return 0;
}

