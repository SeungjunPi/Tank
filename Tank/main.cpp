#define _CRTDBG_MAP_ALLOC

#include <string>
#include <stdlib.h>
#include <crtdbg.h>

#include <cstdio>

#include "Global.h"
#include "Game.h"
#include <atlconv.h>



int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Global::Initiate();

	printf_s("ID: ");
	char id[20];
	
	char* res = fgets(id, 16, stdin);
	if (res == NULL) {
		feof(stdin);
		ferror(stdin);
	}
	id[strnlen_s(id, 20) - 1] = '\0';
	

	char pw[20];
	printf_s("PW: ");
	char* r = fgets(pw, 16, stdin);
	if (r == NULL) {
		feof(stdin);
		ferror(stdin);
	}
	pw[strnlen_s(pw, 20) - 1] = '\0';

	USES_CONVERSION;
	g_playerID = std::wstring(A2W(id));
	g_password = std::wstring(A2W(pw));



	Game::Initialize();
	
	Game::Start();
	Game::CleanUp();


	Global::Terminate();
	
	return 0;
}


