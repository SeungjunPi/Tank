#include "GameServer.h"
#include "Global.h"
#include "NetCore.h"
#include "GameStruct.h"

#include "PlayerManager.h"
#include "ObjectManager.h"

#include "GameEvent.h"
#include "Tank.h"

void OnSessionCreate(UINT32 sessionID);
void OnSessionDisconnect(UINT32 sessionID);

static BOOL s_isRunning = false;

PlayerManager g_playerManager;
ObjectManager g_objectManager;


static void s_ApplyObjectLogic(ULONGLONG tickDiff);
static void s_CollideObjects(ULONGLONG curTick);
static void s_CleanupDestroyedObjects(ULONGLONG curTick);


void GameServer::Initialize()
{
	CreateNetCore(&g_pNetCore);

	g_playerManager.Initiate(2048);
	g_objectManager.Initiate();


	g_sessionIds = new UINT32[2048];
}

void GameServer::Start()
{
	g_pNetCore->StartNetCore();
	g_pNetCore->StartAccept(OnSessionCreate, OnSessionDisconnect);
	UINT32 senderID = 0;

	s_isRunning = true;
	g_previousGameTick = GetTickCount64();

	
	while (s_isRunning) {
		// Handle Keyboard Events
		ULONGLONG currentTick = GetTickCount64();
		ULONGLONG gameTickDiff = currentTick - g_previousGameTick;

		// Apply NetCore Events
		NetMessageQueue* msgs = g_pNetCore->GetReceiveMessages();

		NetMessage* pMsg = msgs->GetNetMessageOrNull(&senderID);
		while (pMsg != NULL) {
			GamePacket::HandlePacket((BYTE*)pMsg->body, senderID);
			pMsg = msgs->GetNetMessageOrNull(&senderID);
		}
		msgs->Flush();

		// Erase Destroyed Objects
		s_CleanupDestroyedObjects(currentTick);

		// Apply Object Logics
		s_ApplyObjectLogic(gameTickDiff);

		// Collide Objects
		s_CollideObjects(currentTick);

			
		g_previousGameTick = currentTick;

		{
			// Todo: WaitForSingleObject
			ULONGLONG tick = GetTickCount64();
			ULONGLONG tickDiff = tick - currentTick;
			ULONGLONG sleepTick = tickDiff >= TICK_PER_GAME_FRAME ? 0 : TICK_PER_GAME_FRAME - tickDiff;
			Sleep(sleepTick);
		}
	}

	printf("End main thread\n");
}

void GameServer::ActivateEndFlag()
{
	s_isRunning = false;
}

void GameServer::End()
{
	g_pNetCore->EndNetCore();
}

void GameServer::CleanUp()
{
	g_playerManager.Terminate();
	g_objectManager.Terminate();
	delete[] g_sessionIds;
	DeleteNetCore(g_pNetCore);
}

void GameServer::Broadcast(BYTE* msg, int len)
{
	UINT16 numPlayers = g_playerManager.GetAllKeys(g_sessionIds);
	g_pNetCore->SendMessageTo(g_sessionIds, numPlayers, msg, len);
}


void OnSessionCreate(UINT32 sessionID)
{
	printf("New session: %u\n", sessionID);
	Player* pPlayer = g_playerManager.TryCreatePlayer(sessionID);

	const size_t msgSize = sizeof(EGameEventCode) + sizeof(PACKET_SC_PLAYER_ID);
	BYTE pRawMsg[msgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawMsg;
	*pEvCode = GAME_EVENT_CODE_SC_PLAYER_ID;
	PACKET_SC_PLAYER_ID* pScPlayerId = (PACKET_SC_PLAYER_ID*)(pRawMsg + sizeof(EGameEventCode));
	pScPlayerId->id = sessionID;
	g_pNetCore->SendMessageTo(sessionID, pRawMsg, msgSize);
	GamePacket::SendSnapshot(sessionID);
}


void OnSessionDisconnect(UINT32 sessionID)
{
	printf("Session %u disconnected\n", sessionID);

	Player* pPlayer = g_playerManager.GetPlayer(sessionID);
	g_playerManager.TryDeletePlayer(sessionID);
	Tank* pTank = g_objectManager.GetTank(sessionID);
	if (pTank != nullptr) {
		g_objectManager.RemoveTank(pTank->GetID(), sessionID);
	}
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
				g_objectManager.RemoveObject(kind, keys[i]);
			}
		}

	}
}


void s_ApplyObjectLogic(ULONGLONG tickDiff)
{
	UINT keys[1024];
	int countKeys;

	int objectKindEnumMax = (int)GAME_OBJECT_KIND_OBSTACLE;
	for (int i = 0; i <= objectKindEnumMax; ++i) {
		EGameObjectKind kind = (EGameObjectKind)i;
		g_objectManager.GetKeys(kind, keys, &countKeys);
		for (int i = 0; i < countKeys; ++i) {
			GameObject* pObject = g_objectManager.GetObjectPtrOrNull(kind, keys[i]);
			if (pObject == NULL) {
				__debugbreak();
			}

			pObject->OnFrame(tickDiff);
		}
	}
}


void s_CollideObjects(ULONGLONG currentTick)
{
	// consider hit per projectile
	UINT projectileKeys[1024];
	UINT otherObjectKeys[1024];
	int numProjectiles;
	g_objectManager.GetKeys(GAME_OBJECT_KIND_PROJECTILE, projectileKeys, &numProjectiles);
	for (int i = 0; i < numProjectiles; ++i) {
		GameObject* pProjectile = g_objectManager.GetObjectPtrOrNull(GAME_OBJECT_KIND_PROJECTILE, projectileKeys[i]);
		if (pProjectile == nullptr) {
			__debugbreak();
		}

		{
			// Tank 

			int numObjects;
			EGameObjectKind kind = EGameObjectKind::GAME_OBJECT_KIND_TANK;

			g_objectManager.GetKeys(kind, otherObjectKeys, &numObjects);
			for (int k = 0; k < numObjects; ++k) {

				GameObject* pOtherObj = g_objectManager.GetObjectPtrOrNull(kind, otherObjectKeys[k]);
				if (pOtherObj == nullptr) {
					__debugbreak();
				}

				Vector3 projectilePosition = pProjectile->GetPosition();
				Vector3 otherObjPosition = pOtherObj->GetPosition();
				float distanceSquared = Vector3::DistanceSquared(projectilePosition, otherObjPosition);

				float collisionTolerance = pProjectile->GetColliderSize() + pOtherObj->GetColliderSize();

				if (distanceSquared <= collisionTolerance * collisionTolerance) {
					pProjectile->OnHit(currentTick);
					pOtherObj->OnHit(currentTick);

					GamePacket::BroadcastTankHit(pOtherObj->GetID(), pProjectile->GetID());
				}
			}
		}

		{
			// Obstacle

			int numObjects;
			EGameObjectKind kind = GAME_OBJECT_KIND_OBSTACLE;

			g_objectManager.GetKeys(kind, otherObjectKeys, &numObjects);
			for (int k = 0; k < numObjects; ++k) {

				GameObject* pOtherObj = g_objectManager.GetObjectPtrOrNull(kind, otherObjectKeys[i]);
				if (pOtherObj == nullptr) {
					__debugbreak();
				}

				Vector3 projectilePosition = pProjectile->GetPosition();
				Vector3 otherObjPosition = pOtherObj->GetPosition();
				float distanceSquared = Vector3::DistanceSquared(projectilePosition, otherObjPosition);

				float collisionTolerance = pProjectile->GetColliderSize() + pOtherObj->GetColliderSize();

				if (distanceSquared <= collisionTolerance * collisionTolerance) {
					pProjectile->OnHit(currentTick);
					pOtherObj->OnHit(currentTick);
					
					//GamePacket::BroadcastTankHit(pOtherObj->GetID(), pProjectile->GetID());
				}
			}
		}
		


	}



}
