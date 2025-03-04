#include "SentinelPch.h"


#include "Collider.h"
#include "ServerPacketHandler.h"
#include "GameServer.h"
#include "GameStruct.h"
#include "Global.h"
#include "IStableFlow.h"
#include "JunDB.h"

#include "ObjectManager.h"
#include "PlayerManager.h"
#include "Tank.h"

// NetCore
#include "INetCore.h"
#include "NetSessionEventQueue.h"
#include "NetMessage.h"
#include "NetMessageQueue.h"



static BOOL s_isRunning = false;


static void s_ApplyObjectLogic(ULONGLONG tickDiff);
static void s_CleanupDestroyedObjects(ULONGLONG curTick);
static void s_OnSessionEvent(UINT32 sessionID, ESessionEvent sessionEvent);
static void s_ProcessDBQueryResults();
// static void s_PreProcessNextMovements();

static void s_ProcessDBResultValidation(DBQueryValidation* pQueryValidation);
static void s_ProcessDBResultLoadScore(DBQueryLoadStat* pQueryLoadStat); // 현재 이 함수는 Validation이 성공한 직후에만 불린다고 가정.
static void s_ProcessDBResultUpdateScore(DBQueryUpdateStat* pQueryUpdateStat);


void OnSessionCreate(UINT32 sessionID);
void OnSessionDisconnect(UINT32 sessionID);




void GameServer::Initialize()
{
	CreateNetCore(&g_pNetCore);
	CreateJunDB(&g_pJunDB);
	CreateStableFlow(&g_pStableFlow);

	ServerPacketHandler::RegisterCallbacks();

	g_playerManager.Initiate(2048);
	g_objectManager.Initiate();

	g_sessionIds = DNew UINT32[2048];
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
		ULONGLONG loopInitTick = GetTickCount64();
		g_currentGameTick = loopInitTick;
		g_diffGameTick = g_currentGameTick - g_previousGameTick;
		
		
		
		ULONGLONG gameTickDiff = loopInitTick - g_previousGameTick;


		// Input Event (DB Result, Session Event, User act, etc..)
		//// Handle NetCore Session Events
		NetSessionEventQueue* pNetSessionEventQueue = g_pNetCore->StartHandleSessionEvents();
		ESessionEvent sessionEvent = pNetSessionEventQueue->GetNetSessionEvent(&senderID);
		while (sessionEvent != ESessionEvent::NONE) {
			s_OnSessionEvent(senderID, sessionEvent);
			sessionEvent = pNetSessionEventQueue->GetNetSessionEvent(&senderID);
		}
		g_pNetCore->EndHandleSessionEvents();

		//// Handle NetCore Messages
		NetMessageQueue* msgs = g_pNetCore->StartHandleReceivedMessages();

		NetMessage* pMsg = msgs->GetNetMessageOrNull(&senderID);
		while (pMsg != NULL) {
			PacketHandler::DispatchPacket((BYTE*)pMsg->body, senderID);
			pMsg = msgs->GetNetMessageOrNull(&senderID);
		}
		g_pNetCore->EndHandleReceivedMessages();


		ULONGLONG netCoreEndTick = GetTickCount64();

		//// DB 
		s_ProcessDBQueryResults();

		ULONGLONG dbEndTick = GetTickCount64();

		// Physics
		g_pStableFlow->ProcessStableFlow(g_currentGameTick);
		ULONGLONG physicsEndTick = GetTickCount64();
		
		// Game Logic(DB, Apply Collision, )
		s_ApplyObjectLogic(gameTickDiff);

		ULONGLONG customLogicEndTick = GetTickCount64();
		
		// Destroy Game Objects
		s_CleanupDestroyedObjects(loopInitTick);

		ULONGLONG cleanUpEndTick = GetTickCount64();
		
		
		if (cleanUpEndTick - loopInitTick > 16) {
			ULONGLONG netCoreDuration = netCoreEndTick - loopInitTick;
			ULONGLONG dbDuration = dbEndTick - netCoreEndTick;
			ULONGLONG physicsDuration = physicsEndTick - dbEndTick;
			ULONGLONG customLogicDutation = customLogicEndTick - physicsEndTick;
			ULONGLONG cleanUpDuration = cleanUpEndTick - customLogicEndTick;
			printf("N[%llu], D[%llu], P[%llu], L[%llu], C[%llu]\n", netCoreDuration, dbDuration, physicsDuration, customLogicDutation, cleanUpDuration);
		}

		g_previousGameTick = loopInitTick;

		{
			// Todo: WaitForSingleObject

			ULONGLONG tick = GetTickCount64() + 1;
			ULONGLONG tickDiff = tick - loopInitTick;
			ULONGLONG sleepTick = tickDiff >= TICK_PER_GAME_FRAME ? 0 : TICK_PER_GAME_FRAME - tickDiff;
			if (tickDiff < TICK_PER_GAME_FRAME) {
				Sleep(TICK_PER_GAME_FRAME - tickDiff);	
				continue;
			}
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
	g_pJunDB->End();
}

void GameServer::CleanUp()
{
	
	g_playerManager.Terminate();
	g_objectManager.Terminate();
	delete[] g_sessionIds;
	
	DeleteNetCore(g_pNetCore);
	TerminateJunDB(g_pJunDB);
	DeleteStableFlow(g_pStableFlow);
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
		printf("RemoveTank: owner=%u, tankId=%u\n", userDBIndex, pTank->GetID().key);
		ServerPacketHandler::BroadcastDeleteTank(pTank->GetID());
		g_objectManager.RemoveTank(pTank->GetID(), userDBIndex);
	}
}


void s_CleanupDestroyedObjects(ULONGLONG curTick)
{
	UINT32 keys[1024];
	UINT32 numObj;

	for (int i = 0; i < (int)GAME_OBJECT_TYPE_MAX; ++i) {
		EGameObjectType type = (EGameObjectType)i;
		g_objectManager.GetKeys(type, keys, &numObj);

		for (UINT32 i = 0; i < numObj; ++i) {
			GameObject* pObj = g_objectManager.GetObjectPtrOrNull(ObjectID{ type, (ObjectKey)keys[i] });
			if (pObj == nullptr) {
				__debugbreak();
			}

			if (pObj->IsDestroyed(curTick)) {
				g_objectManager.RemoveObject(ObjectID{ type, (ObjectKey)keys[i] });
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

//void s_PreProcessNextMovements()
//{
//	UINT32 keys[1024];
//	UINT32 countKeys;
//
//	int objectKindEnumMax = (int)GAME_OBJECT_TYPE_OBSTACLE;
//	for (int i = 0; i <= objectKindEnumMax; ++i) {
//		EGameObjectType kind = (EGameObjectType)i;
//		g_objectManager.GetKeys(kind, keys, &countKeys);
//		for (int j = 0; j < countKeys; ++j) {
//			GameObject* pObject = g_objectManager.GetObjectPtrOrNull(kind, keys[j]);
//			if (pObject == NULL) {
//				__debugbreak();
//			}
//
//			pObject->PreProcessMovementState();
//		}
//	}
//}

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
	ServerPacketHandler::SendLogin(sessionID, true, dbIndex, loadScoreResult.hitCount, loadScoreResult.killCount, loadScoreResult.deathCount);
	
	// Send snapshot
	ServerPacketHandler::SendSnapshot(sessionID);

	// Create players tank
	Tank* pTank = g_objectManager.CreateTank(dbIndex);

	pPlayer->SetTank(pTank);

	ServerPacketHandler::BroadcastCreateTank(pTank->GetID(), dbIndex, pTank->GetTransformPtr());
}

void s_ProcessDBResultUpdateScore(DBQueryUpdateStat* pQueryUpdateStat)
{
}


void s_ApplyObjectLogic(ULONGLONG tickDiff)
{
	UINT32 keys[1024];
	UINT32 countKeys;

	int objectKindEnumMax = (int)GAME_OBJECT_TYPE_MAX;
	for (int i = 0; i < objectKindEnumMax; ++i) {
		EGameObjectType type = (EGameObjectType)i;
		g_objectManager.GetKeys(type, keys, &countKeys);
		for (UINT32 i = 0; i < countKeys; ++i) {
			GameObject* pObject = g_objectManager.GetObjectPtrOrNull(ObjectID{ type, (ObjectKey)keys[i] });
			if (pObject == NULL) {
				__debugbreak();
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
