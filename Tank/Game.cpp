#include "Game.h"

#include "Global.h"
#include "ConsoleRenderer.h"
#include "KeyboardEventListener.h"
#include "AllocObjectManager.h"

#include "DotObject.h"
#include "Tank.h"

#include "NetCore.h"
#include "GameEvent.h"

void CallbackSessionCreate(UINT32 sessionId);
void CallbackSessionDisconnected(UINT32 sessionId);

static void s_ApplyKeyboardEvents(ULONGLONG tickDiff);
static void s_ApplyObjectLogic(ULONGLONG tickDiff);
static void s_SyncOwnTankTransform();
static void s_HandleNetEvents();
static void s_CollideObjects(ULONGLONG curTick);
static BOOL s_IsShootable();
static void s_CleanupDestroyedObjects(ULONGLONG curTick);


static bool s_bIsShootable = true;

static BOOL s_isRunning;

static BOOL s_isMoving;

void Game::Initialize()
{
	g_pPlayerTank = NULL;
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

	g_serverId = g_pNetCore->ConnectTo("127.0.0.1", 30283);
}

void Game::CleanUp()
{
	s_isRunning = false;

	// Todo: disconnect to server

	
	g_pNetCore->EndNetCore();
	
	DeleteNetCore(g_pNetCore);
	ConsoleRenderer::Terminate();
	KeyboardEventListener::Terminate();
	g_objectManager.Terminate();
}

void Game::Start()
{
	g_previousGameTick = GetTickCount64();

	while (s_isRunning) {
		g_currentGameTick = GetTickCount64();
		ULONGLONG gameTickDiff = g_currentGameTick - g_previousGameTick;
		
		if (gameTickDiff > TICK_PER_GAME_FRAME) {
			// Apply Net Events
			s_HandleNetEvents();

			// Get Keyboard Inputs
			s_ApplyKeyboardEvents(gameTickDiff);

			s_CleanupDestroyedObjects(g_currentGameTick);

			// Apply Game Logic(Terminate Game Objects, etc...)
			s_ApplyObjectLogic(gameTickDiff);

			// Apply Physics(Boundary Yank, Hit, etc...)
			//s_CollideObjects(currentTick);

			g_previousGameTick = g_currentGameTick;
		}
		// Draw Call
		ConsoleRenderer::Render();
	}
}

void Game::End()
{
	
}

void CallbackSessionCreate(UINT32 sessionId)
{
}

void CallbackSessionDisconnected(UINT32 sessionId)
{
}

void s_ApplyKeyboardEvents(ULONGLONG tickDiff)
{
	KeyboardEventListener::ProcessInputs();

	if (KeyboardEventListener::inputs.escape) {
		s_isRunning = false;
		return;
	}

	if (g_pPlayerTank != NULL) {
		char startFlag;
		char endFlag;
		char movingFlag;
		KeyboardEventListener::GetAndUpdateKeyboardMovementStatus(&startFlag, &endFlag, &movingFlag);
		if (startFlag) {
			if (startFlag & KEYBOARD_INPUT_FLAG_W) {
				g_pPlayerTank->StartMove(EMOVEMENT::FORWARD);
			} 
			if (startFlag & KEYBOARD_INPUT_FLAG_S) {
				g_pPlayerTank->StartMove(EMOVEMENT::BACKWARD);
			}
			if (startFlag & KEYBOARD_INPUT_FLAG_A) {
				g_pPlayerTank->StartRotate(EROTATION::LEFT);
			}
			if (startFlag & KEYBOARD_INPUT_FLAG_D) {
				g_pPlayerTank->StartRotate(EROTATION::RIGHT);
			}
			GamePacket::SendStartMove(g_pPlayerTank->GetTransformPtr(), startFlag);
			g_lastOwnTankSyncTick = g_currentGameTick;
		}

		if (endFlag) {
			if (endFlag & KEYBOARD_INPUT_FLAG_W) {
				g_pPlayerTank->EndMove(EMOVEMENT::FORWARD);
			}
			if (endFlag & KEYBOARD_INPUT_FLAG_S) {
				g_pPlayerTank->EndMove(EMOVEMENT::BACKWARD);
			}
			if (endFlag & KEYBOARD_INPUT_FLAG_A) {
				g_pPlayerTank->EndRotate(EROTATION::LEFT);
			}
			if (endFlag & KEYBOARD_INPUT_FLAG_D) {
				g_pPlayerTank->EndRotate(EROTATION::RIGHT);
			}
			GamePacket::SendEndMove(g_pPlayerTank->GetTransformPtr(), endFlag);
			g_lastOwnTankSyncTick = g_currentGameTick;
		}

		if (movingFlag) {
			s_SyncOwnTankTransform();
		}

		if (KeyboardEventListener::inputs.shoot) {
			if (s_IsShootable()) {
				Transform transf = { 0.f, };
				g_pPlayerTank->GetTurretInfo(&transf);
				GamePacket::SendShoot(&transf);
			}
		}
	}
}

void s_ApplyObjectLogic(ULONGLONG tickDiff)
{
	UINT keys[128];
	int countKeys;

	int objectKindEnumMax = (int)GAME_OBJECT_KIND_OBSTACLE;
	for (int i = 0; i <= objectKindEnumMax; ++i) {
		EGameObjectKind kind = (EGameObjectKind)i;
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
	if (g_pPlayerTank == nullptr) {
		return;
	}

	if (g_currentGameTick - g_lastOwnTankSyncTick < TICK_OWN_TANK_SYNC) {
		return;
	}

	g_lastOwnTankSyncTick = g_currentGameTick;
	GamePacket::SendMoving(g_pPlayerTank->GetTransformPtr());
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

void s_CollideObjects(ULONGLONG currentTick)
{
	// consider hit per projectile
	UINT projectileKeys[32];
	UINT otherObjectKeys[32];
	int numProjectiles;
	g_objectManager.GetKeys(GAME_OBJECT_KIND_PROJECTILE, projectileKeys, &numProjectiles);
	for (int i = 0; i < numProjectiles; ++i) {
		GameObject* pProjectile = g_objectManager.GetObjectPtrOrNull(GAME_OBJECT_KIND_PROJECTILE, projectileKeys[i]);
		if (pProjectile == nullptr) {
			__debugbreak();
		}

		for (int j = 0; j <= (int)GAME_OBJECT_KIND_OBSTACLE; ++j) {
			int numObjects;
			EGameObjectKind kind = (EGameObjectKind)j;
			if (kind == GAME_OBJECT_KIND_PROJECTILE) {
				continue;
			}

			g_objectManager.GetKeys(kind, otherObjectKeys, &numObjects);

			for (int k = 0; k < numObjects; ++k) {

				GameObject* pOtherObj = g_objectManager.GetObjectPtrOrNull(kind, otherObjectKeys[k]);
				if (pOtherObj == nullptr) {
					__debugbreak();
				}

				if (!pOtherObj->IsAlive()) {
					continue;
				}

				Vector3 projectilePosition = pProjectile->GetPosition();
				Vector3 otherObjPosition = pOtherObj->GetPosition();
				float distanceSquared = Vector3::DistanceSquared(projectilePosition, otherObjPosition);

				float collisionTolerance = pProjectile->GetColliderSize() + pOtherObj->GetColliderSize();

				if (distanceSquared <= collisionTolerance * collisionTolerance) {
					pProjectile->OnHit(currentTick);
					pOtherObj->OnHit(currentTick);
				}
			}
		}
		
		
	}


	
}

BOOL s_IsShootable()
{
	static ULONGLONG lastShoot = 0;
	const static ULONGLONG SHOOT_COOL_DOWN = 500;

	if (g_pPlayerTank == nullptr) {
		return false;
	}

	if (g_previousGameTick - lastShoot > SHOOT_COOL_DOWN) {
		s_bIsShootable = true;
		lastShoot = g_previousGameTick;
		return true;
	}

	return false;
}

void s_CleanupDestroyedObjects(ULONGLONG curTick)
{
	UINT keys[1024];
	int numObj;

	for (int i = 0; i <= (int)GAME_OBJECT_KIND_OBSTACLE; ++i) {
		EGameObjectKind kind = (EGameObjectKind)i;
		g_objectManager.GetKeys(kind, keys, &numObj);

		for (int i = 0; i < numObj; ++i) {
			GameObject* pObj = g_objectManager.GetObjectPtrOrNull(kind, keys[i]);
			if (pObj == nullptr) {
				__debugbreak();
			}

			if (pObj->IsDestroyed(curTick)) {
				if (pObj == g_pPlayerTank) {
					g_pPlayerTank = nullptr;
				}
				g_objectManager.RemoveObject(kind, keys[i]);
			}
		}

	}
}
