#include "DummyGame.h"
#include "DummyGlobal.h"
#include "DummyManager.h"

#include "Global.h"

#include "ConsoleRenderer.h"
#include "CollisionManager.h"
#include "KeyboardEventListener.h"
#include "AllocObjectManager.h"

#include "Tank.h"

#include "INetCore.h"
#include "NetSessionEventQueue.h"
#include "NetMessage.h"
#include "NetMessageQueue.h"

#include "DummyPacketHandler.h"
#include "IStableFlow.h"
#include "Collider.h"
#include "Player.h"
#include "Camera.h"


static void s_ApplyKeyboardEvents(ULONGLONG tickDiff);
static void s_ApplyObjectLogic(ULONGLONG tickDiff);
static void s_HandleNetEvents();
static void s_CleanupDestroyedObjects(ULONGLONG curTick);

static BOOL s_isRunning;

void DummyGame::Initialize()
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

	DummyPacketHandler::RegisterCallbacks();

	CreateStableFlow(&g_pStableFlow);

	g_pCamera = DNew GameCamera;
	g_pDummyManager = DNew DummyManager();
	g_pDummyManager->Initiate();
}

void DummyGame::CleanUp()
{
	s_isRunning = false;
	// Todo: disconnect to server

	g_pNetCore->EndNetCore();

	DeleteStableFlow(g_pStableFlow);
	DeleteNetCore(g_pNetCore);

	ConsoleRenderer::Terminate();
	KeyboardEventListener::Terminate();
	g_objectManager.Terminate();
	g_pDummyManager->Shutdown();

	delete g_pDummyManager;
	delete g_pCamera;
}

void DummyGame::Start()
{
	g_previousGameTick = GetTickCount64();

	while (s_isRunning) {
		g_currentGameTick = GetTickCount64();
		g_gameTickDiff = g_currentGameTick - g_previousGameTick;
		ULONGLONG gameTickDiff = g_currentGameTick - g_previousGameTick;

		if (gameTickDiff > TICK_PER_GAME_FRAME) {
			// Input Event (Keyboard, NetEvent, etc..)
			//// Handle NetCore Messages
			s_HandleNetEvents();

			// Get Keyboard Inputs
			s_ApplyKeyboardEvents(gameTickDiff);

			g_pDummyManager->Tick();


			// Physics(Detect Collision, Decide Next Movement)
			g_pStableFlow->ProcessStableFlow(g_currentGameTick);

			// Game Logic()
			s_ApplyObjectLogic(gameTickDiff);

			// Destroy Game Objects
			s_CleanupDestroyedObjects(g_currentGameTick);

			g_previousGameTick = g_currentGameTick;
		}
		// Draw Call
		//ConsoleRenderer::Render();
	}
}

void DummyGame::Shutdown()
{
	s_isRunning = false;
	// TODO: clean memory
}

void DummyGame::End()
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
}

void s_ApplyObjectLogic(ULONGLONG tickDiff)
{
	UINT32 keys[1024];
	UINT32 countKeys;

	int objectKindEnumMax = (int)GAME_OBJECT_TYPE_MAX;
	for (int i = 0; i < objectKindEnumMax; ++i) {
		EGameObjectType kind = (EGameObjectType)i;
		g_objectManager.GetKeys(kind, keys, &countKeys);
		for (int j = 0; j < countKeys; ++j) {
			GameObject* pObject = g_objectManager.GetObjectPtrOrNull(kind, keys[j]);
			if (pObject == NULL) {
				__debugbreak();
			}
			
			if (pObject->GetOwnerID() == 100) {

				//printf("ClientID: %u\n", pObject->GetOwnerID());
				//const Transform* pTrans = pObject->GetTransformPtr();
				//printf("Pos:[%f, %f, %f]\n", pTrans->Position.x, pTrans->Position.y, pTrans->Position.z);
				//printf("Rot:[%f, %f, %f %f]\n", pTrans->Rotation.w, pTrans->Rotation.x, pTrans->Rotation.y, pTrans->Rotation.z);
				//Transform colliderTransform;
				//Vector3 velocity;
				//Vector3 angularVelocity;
				//pObject->GetColliderPtr()->GetComputedResults(&colliderTransform, &velocity, &angularVelocity);
				//printf("ColliderNext Pos:[%f, %f, %f]\n", colliderTransform.Position.x, colliderTransform.Position.y, colliderTransform.Position.z);
				//printf("ColliderNext Rot:[%f, %f, %f %f]\n", colliderTransform.Rotation.w, colliderTransform.Rotation.x, colliderTransform.Rotation.y, colliderTransform.Rotation.z);

				//printf("ColliderNext Vel:[%f, %f, %f]\n", velocity.x, velocity.y, velocity.z);
				//printf("ColliderNext AngVel:[%f, %f, %f]\n", angularVelocity.x, angularVelocity.y, angularVelocity.z);
			}
			pObject->Tick(tickDiff);
		}
	}


	JStack* collisionPairs = g_pStableFlow->GetCollisionPairs();
	UINT32 size = collisionPairs->GetCount();
	CollisionPair* pair = (CollisionPair*)collisionPairs->First();
	for (UINT32 i = 0; i < size; ++i) {
		GameObject* pObjA = pair->a->GetAttachedObjectPtr();
		GameObject* pObjB = pair->b->GetAttachedObjectPtr();

		pObjA->OnHitWith(g_currentGameTick, pObjB);
		pObjB->OnHitWith(g_currentGameTick, pObjA);
		++pair;
	}
}

void s_HandleNetEvents()
{
	NetMessageQueue* pReceiveMessageQueue = g_pNetCore->StartHandleReceivedMessages();
	UINT32 serverId;
	NetMessage* pMessage = pReceiveMessageQueue->GetNetMessageOrNull(&serverId);
	while (pMessage != nullptr) {
		PacketHandler::DispatchPacket((BYTE*)pMessage->body, serverId);
		pMessage = pReceiveMessageQueue->GetNetMessageOrNull(&serverId);
	}
	g_pNetCore->EndHandleReceivedMessages();
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
