#include "Game.h"

#include "Global.h"
#include "ConsoleRenderer.h"
#include "KeyboardEventListener.h"
#include "AllocObjectManager.h"

#include "DotObject.h"
#include "Tank.h"

#include "NetCore.h"
#include "GameEvent.h"
#include "ICollisionManager.h"
#include "Collider.h"
#include "Player.h"

#include "Camera.h"

#include "DummyManager.h"

static void s_ApplyKeyboardEvents(ULONGLONG tickDiff);
static void s_ApplyObjectLogic(ULONGLONG tickDiff);
static void s_SyncOwnTankTransform();
static void s_HandleNetEvents();
static void s_CollideObjects();
static void s_CleanupDestroyedObjects(ULONGLONG curTick);


static bool s_bIsShootable = true;

static BOOL s_isRunning;

static BOOL s_isMoving;

void Game::Initialize()
{
	s_isRunning = true;
	KeyboardEventListener::Initiate();

	g_objectManager.Initiate();

	ConsoleRenderer::Initiate(&g_objectManager);
	ConsoleRenderer::TurnOnShowDebugInfo();

	CreateNetCore(&g_pNetCore);
	BOOL res = g_pNetCore->StartNetCore();
	if (res != true) {
		__debugbreak();
	}

	CreateCollisionManager(&g_pCollisionManager);

	g_pCamera = DNew GameCamera;
	g_pDummyManager = DNew DummyManager();
}

void Game::CleanUp()
{
	s_isRunning = false;
	// Todo: disconnect to server

	
	g_pNetCore->EndNetCore();

	DeleteCollisionManager(g_pCollisionManager);
	DeleteNetCore(g_pNetCore);
	
	ConsoleRenderer::Terminate();
	KeyboardEventListener::Terminate();
	g_objectManager.Terminate();
	delete g_pCamera;
}

void Game::Start()
{
	//g_pPlayer = DNew Player(g_userName, g_password);
	//SessionID serverID = g_pNetCore->ConnectTo("127.0.0.1", 30283);
	//g_pPlayer->OnConnected(serverID);

	//// Send Login
	//GamePacket::SendLogin(g_userName, g_password);

	
	

	g_previousGameTick = GetTickCount64();

	while (s_isRunning) {
		g_currentGameTick = GetTickCount64();
		ULONGLONG gameTickDiff = g_currentGameTick - g_previousGameTick;
		
		if (gameTickDiff > TICK_PER_GAME_FRAME) {
			
			// Apply Net Events
			s_HandleNetEvents();

			g_pDummyManager->OnFrame();

			// Get Keyboard Inputs
			s_ApplyKeyboardEvents(gameTickDiff);

			s_CleanupDestroyedObjects(g_currentGameTick);

			// Apply Game Logic(Terminate Game Objects, etc...)
			s_ApplyObjectLogic(gameTickDiff);

			// Apply Physics(Boundary Yank, Hit, etc...)
			s_CollideObjects();

			g_previousGameTick = g_currentGameTick;
		}
		// Draw Call
		//ConsoleRenderer::Render();
	}
}

void Game::Shutdown()
{
	s_isRunning = false;
	// TODO: clean memory
}

void Game::End()
{
	
}

void s_ApplyKeyboardEvents(ULONGLONG tickDiff)
{
	KeyboardEventListener::ProcessInputs();

	UINT64 pressedFlag;
	UINT64 releasedFlag;
	UINT64 heldFlag;
	KeyboardEventListener::GetAndUpdateKeyboardMovementStatus(&pressedFlag, &releasedFlag, &heldFlag);

	if (pressedFlag & KEYBOARD_INPUT_FLAG_ESC) {
		s_isRunning = false;
		return;
	}

	//if (g_pPlayer != nullptr) {
	//	g_pPlayer->HandleKeyboardEvents(pressedFlag, releasedFlag, heldFlag);
	//}	
}

void s_ApplyObjectLogic(ULONGLONG tickDiff)
{
	UINT32 keys[128];
	UINT32 countKeys;

	int objectKindEnumMax = (int)GAME_OBJECT_TYPE_OBSTACLE;
	for (int i = 0; i <= objectKindEnumMax; ++i) {
		EGameObjectType kind = (EGameObjectType)i;
		g_objectManager.GetKeys(kind, keys, &countKeys);
		for (int j = 0; j < countKeys; ++j) {
			GameObject* pObject = g_objectManager.GetObjectPtrOrNull(kind, keys[j]);
			if (pObject == NULL) {
				__debugbreak();
			}

			pObject->OnFrame(tickDiff);
		}
	}
}

void s_SyncOwnTankTransform()
{
	/*if (g_pPlayer->GetTankID().equals(INVALID_OBJECT_ID)) {
		return;
	}

	if (g_currentGameTick - g_lastOwnTankSyncTick < TICK_OWN_TANK_SYNC) {
		return;
	}

	g_lastOwnTankSyncTick = g_currentGameTick;
	GamePacket::SendMoving(g_pPlayerTank->GetTransformPtr());*/
}

void s_HandleNetEvents()
{
	NetMessageQueue* pReceiveMessageQueue = g_pNetCore->StartHandleReceivedMessages();
	UINT32 serverId;
	NetMessage* pMessage = pReceiveMessageQueue->GetNetMessageOrNull(&serverId);
	while (pMessage != nullptr) {
		GamePacket::HandlePacket((BYTE*)pMessage->body, serverId);
		pMessage = pReceiveMessageQueue->GetNetMessageOrNull(&serverId);
	}
	g_pNetCore->EndHandleReceivedMessages();
}

void s_CollideObjects()
{
	ColliderID* pColliderIDs = nullptr;
	UINT32 countCollidedObjects = g_pCollisionManager->DetectCollision(&pColliderIDs);
	for (UINT32 i = 0; i < countCollidedObjects; ++i) {
		Collider* pCollider = g_pCollisionManager->GetAttachedColliderPtr(pColliderIDs[i]);
		if (pCollider == nullptr) {
			continue;
		}

		GameObject* pGameObject = pCollider->GetAttachedObjectPtr();
		pGameObject->OnHit(g_currentGameTick);
	}
}

void s_CleanupDestroyedObjects(ULONGLONG curTick)
{
	UINT32 keys[1024];
	UINT32 numObj;

	for (int i = 0; i < (int)GAME_OBJECT_TYPE_MAX; ++i) {
		EGameObjectType type = (EGameObjectType)i;
		g_objectManager.GetKeys(type, keys, &numObj);

		for (int i = 0; i < numObj; ++i) {
			GameObject* pObj = g_objectManager.GetObjectPtrOrNull(type, keys[i]);
			if (pObj == nullptr) {
				__debugbreak();
			}

			if (pObj->IsDestroyed(curTick)) {
				g_objectManager.RemoveObject(type, keys[i]);
			}
		}

	}
}
