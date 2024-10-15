

#include "Global.h"

#include "NetCore.h"
#include "Tank.h"
#include "AllocObjectManager.h"


ULONGLONG g_previousGameTick = 0;
ULONGLONG g_currentGameTick = 0;
DWORD g_currentFPS = 0;

NetCore* g_pNetCore;
UINT32 g_serverId;

bool g_isTankCreateRequest = false;
Tank* g_pPlayerTank;
AllocObjectManager g_objectManager;

UINT32 g_playerId;

void Global::Initiate()
{
	// Todo: 전역적으로 사용하는 초기화가 필요한 변수들의 초기화를 여기로 옮기기 
}

void Global::Terminate()
{
	// Todo: 위 Initiate에서 사용한 변수들 여기서 해제
}
