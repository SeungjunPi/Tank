#include "GameServer.h"
#include "Global.h"
#include "NetCore.h"
#include "GameStruct.h"

#include "PlayerManager.h"
#include "ObjectManager.h"

#include "GameEvent.h"
#include "Tank.h"
#include "JunDB.h"



static BOOL s_isRunning = false;

PlayerManager g_playerManager;
ObjectManager g_objectManager;


static void s_ApplyObjectLogic(ULONGLONG tickDiff);
static void s_CollideObjects(ULONGLONG curTick);
static void s_CleanupDestroyedObjects(ULONGLONG curTick);
static void s_OnSessionEvent(UINT32 sessionID, ESessionEvent sessionEvent);
static void s_ProcessDBQueryResults();

static void s_ProcessDBResultValidation(DBQueryValidation* pQueryValidation);
static void s_ProcessDBResultLoadScore(DBQueryLoadStat* pQueryLoadStat); // 현재 이 함수는 Validation이 성공한 직후에만 불린다고 가정.
static void s_ProcessDBResultUpdateScore(DBQueryUpdateStat* pQueryUpdateStat);


void OnSessionCreate(UINT32 sessionID);
void OnSessionDisconnect(UINT32 sessionID);




void GameServer::Initialize()
{
	CreateNetCore(&g_pNetCore);
	CreateJunDB(&g_pJunDB);

	g_playerManager.Initiate(2048);
	g_objectManager.Initiate();

	g_sessionIds = new UINT32[2048];
}

void GameServer::Start()
{
	g_pNetCore->StartNetCore();
	g_pNetCore->StartAccept();


	//// temporal ////
	DBConnectionInfo dbConnectionInfo;
	dbConnectionInfo.ip = "127.0.0.1";
	dbConnectionInfo.port = "1434";
	dbConnectionInfo.dbName = "tankDB";
	dbConnectionInfo.uid = "pearson";
	dbConnectionInfo.pwd = "mathematics";
	//////////////////

	g_pJunDB->Start(dbConnectionInfo, 1);
	
	UINT32 senderID = 0;

	s_isRunning = true;
	g_previousGameTick = GetTickCount64();

	
	while (s_isRunning) {
		// Handle Keyboard Events
		ULONGLONG currentTick = GetTickCount64();
		g_currentGameTick = currentTick;
		ULONGLONG gameTickDiff = currentTick - g_previousGameTick;

		// Handle NetCore Session Events
		NetSessionEventQueue* pNetSessionEventQueue = g_pNetCore->StartHandleSessionEvents();
		ESessionEvent sessionEvent = pNetSessionEventQueue->GetNetSessionEvent(&senderID);
		while (sessionEvent != ESessionEvent::NONE) {
			s_OnSessionEvent(senderID, sessionEvent);
			sessionEvent = pNetSessionEventQueue->GetNetSessionEvent(&senderID);
		}
		g_pNetCore->EndHandleSessionEvents();

		// Handle NetCore Messages
		NetMessageQueue* msgs = g_pNetCore->StartHandleReceivedMessages();

		NetMessage* pMsg = msgs->GetNetMessageOrNull(&senderID);
		while (pMsg != NULL) {
			GamePacket::HandlePacket((BYTE*)pMsg->body, senderID);
			pMsg = msgs->GetNetMessageOrNull(&senderID);
		}
		g_pNetCore->EndHandleReceivedMessages();

		// DB 
		s_ProcessDBQueryResults();

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
	UINT16 numPlayers = g_playerManager.GetAllSessionIDs(g_sessionIds);
	g_pNetCore->SendMessageTo(g_sessionIds, numPlayers, msg, len);
}

void GameServer::BroadcastExcept(BYTE* msg, int len, UINT32 sessionId)
{
	UINT16 numPlayers = g_playerManager.GetAllSessionIDs(g_sessionIds);
	for (UINT16 i = 0; i < numPlayers; ++i) {
		if (g_sessionIds[i] != sessionId) {
			g_pNetCore->SendMessageTo(g_sessionIds[i], msg, len);
		}
	}
}


void OnSessionCreate(UINT32 sessionID)
{
	printf("New session: %u\n", sessionID);
	//Player* pPlayer = g_playerManager.TryCreatePlayer(sessionID);

	//const size_t msgSize = sizeof(EGameEventCode) + sizeof(PACKET_SC_PLAYER_ID);
	//BYTE pRawMsg[msgSize] = { 0, };
	//EGameEventCode* pEvCode = (EGameEventCode*)pRawMsg;
	//*pEvCode = GAME_EVENT_CODE_SC_PLAYER_ID;
	//PACKET_SC_PLAYER_ID* pScPlayerId = (PACKET_SC_PLAYER_ID*)(pRawMsg + sizeof(EGameEventCode));
	//pScPlayerId->id = sessionID;
	//g_pNetCore->SendMessageTo(sessionID, pRawMsg, msgSize);
	//GamePacket::SendSnapshot(sessionID);


	//{
	//	// Auto create user's tank
	//	Tank* pTank = g_objectManager.CreateTank(sessionID);

	//	const size_t PACKET_SIZE = sizeof(EGameEventCode) + sizeof(PACKET_SC_CREATE_TANK);
	//	BYTE pRawPacket[PACKET_SIZE] = { 0, };

	//	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	//	*pEvCode = GAME_EVENT_CODE_SC_CREATE_TANK;
	//	PACKET_SC_CREATE_TANK* pSCCreateTank = (PACKET_SC_CREATE_TANK*)(pRawPacket + sizeof(EGameEventCode));
	//	pSCCreateTank->objectId = pTank->GetID();
	//	pSCCreateTank->ownerId = sessionID;
	//	// 
	//	memcpy(&pSCCreateTank->transform, pTank->GetTransformPtr(), sizeof(Transform));

	//	printf("CreateTank: owner=%u, tankId=%u\n", sessionID, pSCCreateTank->objectId);

	//	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
	//}
}


void OnSessionDisconnect(UINT32 sessionID)
{
	printf("Session %u disconnected\n", sessionID);

	Player* pPlayer = g_playerManager.GetPlayerBySessionID(sessionID);
	if (pPlayer == nullptr) {
		// Validation 실패, 또는 Validation 이전에 종료된 session.
		return;
	}

	UserDBIndex userDBIndex = pPlayer->GetUserIndex();
	TankScore score = pPlayer->GetScore();

	g_pJunDB->UpdateStat(userDBIndex, score.hitCount, score.killCount, score.deathCount);

	g_playerManager.TryDeletePlayerBySessionID(sessionID);
	
	Tank* pTank = g_objectManager.GetTankByOwnerId(userDBIndex);
	if (pTank != nullptr) {
		printf("RemoveTank: owner=%u, tankId=%u\n", userDBIndex, pTank->GetID());
		GamePacket::BroadcastDeleteTank(pTank->GetID());
		g_objectManager.RemoveTank(pTank->GetID(), userDBIndex);
	}
}


void s_CleanupDestroyedObjects(ULONGLONG curTick)
{
	ObjectID keys[1024];
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

void s_OnSessionEvent(UINT32 sessionID, ESessionEvent sessionEvent)
{
	switch (sessionEvent) {
	case ESessionEvent::CREATE_PASSIVE_CLIENT:
		OnSessionCreate(sessionID);
		break;
	case ESessionEvent::DELETE_PASSIVE_CLIENT:
		OnSessionDisconnect(sessionID);
		break;
	default:
		__debugbreak();
	}
}

void s_ProcessDBQueryResults()
{
	g_pJunDB->BeginHandleResult();
	DBEvent dbEvent;
	while (g_pJunDB->TryGetEvent(&dbEvent)) {
		switch (dbEvent.code) {
		case DBEventCode::QUERY_VALIDATION:
			s_ProcessDBResultValidation((DBQueryValidation*)dbEvent.pEvent);
			break;
		case DBEventCode::QUERY_LOAD_STAT:
			s_ProcessDBResultLoadScore((DBQueryLoadStat*)dbEvent.pEvent);
			break;
		case DBEventCode::QUERY_UPDATE_STAT:
			s_ProcessDBResultUpdateScore((DBQueryUpdateStat*)dbEvent.pEvent);
			break;
		}
	}
	g_pJunDB->EndHandleResult();
}

void s_ProcessDBResultValidation(DBQueryValidation* pQueryValidation)
{
	DBResultUserValidation validationResult;
	pQueryValidation->GetResult(&validationResult);
	UINT32 sessionID = pQueryValidation->GetSessionID();
	const WCHAR* name = pQueryValidation->GetName();
	if (validationResult.code != DBResultCode::SUCCESS) {
		wprintf(L"Validation failed user=%u, name=%s\n", sessionID, name);
		return;
	}
	printf("Validation Successed to session=%u, user=%u\n", sessionID, validationResult.userID);
	// Load Score,
	g_pJunDB->LoadStat(pQueryValidation->GetSessionID(), validationResult.userID);
}

void s_ProcessDBResultLoadScore(DBQueryLoadStat* pQueryLoadStat)
{
	// Handle Failure -> return
	DBResultLoadScore loadScoreResult;
	pQueryLoadStat->GetResult(&loadScoreResult);
	UINT32 sessionID = pQueryLoadStat->GetSessionID();
	UINT32 dbIndex = pQueryLoadStat->GetID();
	if (loadScoreResult.code != DBResultCode::SUCCESS) {
		// validation을 통과했으므로 일반적인 경우에선 실패해선 안됨. 
		wprintf(L"Validation failed userIndex=%u\n", dbIndex);
		__debugbreak();
		return;
	}
	printf("Load Score to user=%u complete.\n", dbIndex);
	
	Player* pPlayer = g_playerManager.TryCreatePlayer(sessionID, dbIndex, loadScoreResult.hitCount, loadScoreResult.killCount, loadScoreResult.deathCount);

	// Send Success to Login Message
	const size_t msgSize = sizeof(EGameEventCode) + sizeof(PACKET_SC_LOGIN);
	BYTE pRawMsg[msgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawMsg;
	*pEvCode = GAME_EVENT_CODE_SC_LOGIN;
	PACKET_SC_LOGIN* pScLogin = (PACKET_SC_LOGIN*)(pRawMsg + sizeof(EGameEventCode));
	pScLogin->result = true;
	pScLogin->userDBIndex = dbIndex;
	pScLogin->hitCount = loadScoreResult.hitCount;
	pScLogin->killCount = loadScoreResult.killCount;
	pScLogin->deathCount = loadScoreResult.deathCount;
	g_pNetCore->SendMessageTo(sessionID, pRawMsg, msgSize);
	
	// Send snapshot
	GamePacket::SendSnapshot(sessionID);

	
	{
		// Create players tank
		Tank* pTank = g_objectManager.CreateTank(dbIndex);
		const size_t PACKET_SIZE = sizeof(EGameEventCode) + sizeof(PACKET_SC_CREATE_TANK);
		BYTE pRawPacket[PACKET_SIZE] = { 0, };

		EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
		*pEvCode = GAME_EVENT_CODE_SC_CREATE_TANK;
		PACKET_SC_CREATE_TANK* pSCCreateTank = (PACKET_SC_CREATE_TANK*)(pRawPacket + sizeof(EGameEventCode));
		pSCCreateTank->objectId = pTank->GetID();
		pSCCreateTank->ownerId = dbIndex;
		// 
		memcpy(&pSCCreateTank->transform, pTank->GetTransformPtr(), sizeof(Transform));

		printf("CreateTank: owner=%u, tankId=%u\n", dbIndex, pSCCreateTank->objectId);

		GameServer::Broadcast(pRawPacket, PACKET_SIZE);
	}
}

void s_ProcessDBResultUpdateScore(DBQueryUpdateStat* pQueryUpdateStat)
{
}


void s_ApplyObjectLogic(ULONGLONG tickDiff)
{
	ObjectID keys[1024];
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
	ObjectID projectileKeys[1024];
	ObjectID otherObjectKeys[1024];
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

					UserDBIndex hitterIndex = pProjectile->GetOwnerId();
					g_playerManager.IncreaseKillCount(hitterIndex);

					UserDBIndex victimIndex = pOtherObj->GetOwnerId();
					g_playerManager.IncreaseDeathCount(victimIndex);
					

					GamePacket::BroadcastTankHit(pOtherObj->GetID(), pProjectile->GetID(), pProjectile->GetOwnerId(), pOtherObj->GetOwnerId());
					printf("Tank Hit: shooter=%u, target=%u, tankId=%u, projectileId=%d\n", pProjectile->GetOwnerId(), pOtherObj->GetOwnerId(), pOtherObj->GetID(), pProjectile->GetID());
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
