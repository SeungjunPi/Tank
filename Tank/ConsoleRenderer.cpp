#include "Global.h"

#include <windows.h>
#include <thread>
#include <ctime>

#include "ConsoleRenderer.h"
#include "GameObject.h"
#include "ObjectManager.h"
#include "Camera.h"

AllocObjectManager* ConsoleRenderer::_pAllocObjManager = nullptr;


struct GameMap
{
	SHORT xMin;
	SHORT xMax;
	SHORT yMin;
	SHORT yMax;
	SHORT xCenter;
	SHORT yCenter;
};

static HANDLE s_hOut;

static WCHAR* s_backBuffer;

static SHORT s_consoleWidth = 0;
static SHORT s_consoleHeight = 0;

static BOOL s_bInitiated = false;
static BOOL s_bIsDisplayDebugInfo = false;

static GameMap s_gameMap;

static int g_numOfFrame = 0;
static int g_numOfGame = 0;

static CHAR* s_FPSTextInfo = nullptr;

static clock_t s_curTime = 0;
static clock_t s_prevTime = 0;


Vertex* s_vertices = nullptr;


void s_ClearBackBuffer();
void s_PrintDebugInfos();
void s_Draw(SHORT x, SHORT y, WCHAR c);
void s_DrawFPS(SHORT x, SHORT y);
void s_DrawScore(SHORT x, SHORT y);




void ConsoleRenderer::Initiate(AllocObjectManager* pObjectManager)
{
	if (s_bInitiated) {
		return;
	}

	s_hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(s_hOut, &info);

	s_consoleWidth = info.dwSize.X;
	s_consoleHeight = info.dwSize.Y; // ?????????

	CONSOLE_CURSOR_INFO consoleCursorInfo;
	consoleCursorInfo.bVisible = FALSE; // Set Cursor as invisible
	consoleCursorInfo.dwSize = 1;

	SetConsoleCursorInfo(s_hOut, &consoleCursorInfo);

	COORD coor = { 0, 0 };
	DWORD dw;
	FillConsoleOutputCharacter(s_hOut, L'.', s_consoleWidth * s_consoleHeight, coor, &dw);

	s_backBuffer = DNew WCHAR[s_consoleWidth * s_consoleHeight];
	s_ClearBackBuffer();
	
	s_gameMap.xCenter = s_consoleWidth / 2;
	s_gameMap.yCenter = s_consoleHeight / 2;

	s_gameMap.xMax = s_gameMap.xCenter - 1;
	s_gameMap.xMin = -s_gameMap.xMax;
	s_gameMap.yMax = s_gameMap.yCenter - 1;
	s_gameMap.yMin = -s_gameMap.yMax;
		
	s_vertices = DNew Vertex[10];

	_pAllocObjManager = pObjectManager;

	s_bInitiated = true;
}

void ConsoleRenderer::Terminate()
{
	if (!s_bInitiated) {
		return;
	}
	_pAllocObjManager = nullptr;
	CloseHandle(s_hOut);
	delete[] s_backBuffer;
	s_bIsDisplayDebugInfo = false;
	delete[] s_vertices;
}

void ConsoleRenderer::Render()
{
	static ULONGLONG previousTick = GetTickCount64();
	static DWORD frameCount = 0;

	frameCount++;

	ULONGLONG currentTick = GetTickCount64();
	if (currentTick - previousTick > 1000) {
		g_currentFPS = frameCount;
		frameCount = 0;
		previousTick = currentTick;
	}

	s_ClearBackBuffer();

	DrawObjects();

	s_DrawScore(s_consoleWidth - 20, 1);

	// Overwrite Debug Infos
	if (s_bIsDisplayDebugInfo) {
		s_DrawFPS(1, 1);
	}

	DWORD len;
	COORD pos = { 0, };
	WriteConsoleOutputCharacter(s_hOut, s_backBuffer, s_consoleWidth * s_consoleHeight, pos, &len);
}

void ConsoleRenderer::TurnOnShowDebugInfo()
{
	s_bIsDisplayDebugInfo = true;
}

void ConsoleRenderer::TurnOffShowDebugInfo()
{
	if (!s_bIsDisplayDebugInfo) {
		return;
	}
}

void s_PrintDebugInfos()
{
	/*if (!s_bIsDisplayDebugInfo) {
		__debugbreak();
	}
	s_curTime = clock();
	clock_t deltaTime = s_curTime - s_prevTime;

	sprintf_s(s_FPSTextInfo, FPS_BUF_SIZE, "FPS: %d, deltaTime: %ld\n", g_numOfFrame, deltaTime);

	DWORD dw;
	COORD cursorPosition = { 0, 0 };
	WriteConsoleOutputCharacterA(g_hFrontScreen, s_FPSTextInfo, strlen(s_FPSTextInfo), cursorPosition, &dw);
	
	++g_numOfFrame;*/
}


void s_Draw(SHORT x, SHORT y, WCHAR c)
{
	if (x < s_gameMap.xMin || x > s_gameMap.xMax) {
		return;
	}

	if (y < s_gameMap.yMin || y > s_gameMap.yMax) {
		return;
	}

	SHORT screenCoordX = x + s_gameMap.xCenter;
	SHORT screenCoordY = y + s_gameMap.yCenter;

	s_backBuffer[screenCoordX + screenCoordY * s_consoleWidth] = c;
}


void ConsoleRenderer::DrawObjects()
{
	UINT32 countObjects = 0;
	UINT32 keys[128];

	Vector3 cameraPos = g_pCamera->GetPosition();

	int objectKindMax = (int)GAME_OBJECT_TYPE_OBSTACLE;
	for (int i = 0; i < objectKindMax; ++i) {
		EGameObjectType kind = (EGameObjectType)i;

		_pAllocObjManager->GetKeys(kind, keys, &countObjects);

		for (int i = 0; i < countObjects; ++i) {

			UINT numVectors;
			UINT key = keys[i];

			GameObject* pObj = _pAllocObjManager->GetObjectPtrOrNull(kind, key);
			if (!pObj->IsAlive()) {
				continue;
			}


			_pAllocObjManager->GetTransformedModelOf(kind, key, s_vertices, &numVectors);

			Vertex* pVertex = s_vertices;
			for (UINT k = 0; k < numVectors; ++k) {
				s_Draw((SHORT)roundf(pVertex->v.x - cameraPos.x), (SHORT)roundf(pVertex->v.y) - cameraPos.y, pVertex->c);
				pVertex++;
			}
		}
	}
}


//
//
//void PrintScreen(ConsoleMessage* messages, SHORT count)
//{
//	DWORD dw;
//
//	ConsoleMessage* pMessage = messages;
//	for (SHORT i = 0; i < count; ++i) {
//		WriteConsoleOutputCharacterA(s_hOut, pMessage->str, pMessage->strLen, pMessage->printCoor, &dw);
//		pMessage++;
//	}
//
//	//s_FlipScreen();
//}


void s_ClearBackBuffer()
{
	ZeroMemory(s_backBuffer, sizeof(WCHAR) * s_consoleWidth * s_consoleHeight);
}

void s_DrawFPS(SHORT x, SHORT y)
{
	SHORT offset = x + y * s_consoleWidth;
	WCHAR* dst = s_backBuffer + offset;

	WCHAR txt[32];
	DWORD len = swprintf_s(txt, L"FPS:%u", g_currentFPS);

	memcpy(dst, txt, sizeof(WCHAR) * len);
}

void s_DrawScore(SHORT x, SHORT y)
{
	SHORT offset = x + y * s_consoleWidth;
	WCHAR* dst = s_backBuffer + offset;

	WCHAR txt[32];
	DWORD len = swprintf_s(txt, L"Score: (%hd, %hd, %hd)", g_score.hit, g_score.kill, g_score.death);

	memcpy(dst, txt, sizeof(WCHAR) * len);
}


