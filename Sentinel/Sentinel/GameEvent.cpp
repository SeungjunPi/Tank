#include "GameEvent.h"
#include "GameServer.h"
#include "Global.h"
#include "NetCore.h"
#include "Tank.h"
#include "Projectile.h"


BOOL GamePacket::Validate(BYTE* pGameEvent, UINT32 senderId)
{
	return true;
}

void GamePacket::HandlePacket(BYTE* pGameEvent, UINT32 senderId)
{
	EGameEventCode eventCode = *(EGameEventCode*)pGameEvent;
	switch (eventCode) {
	case GAME_EVENT_CODE_CS_CREATE_TANK:
		HandleCreateTank(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_CS_DELETE_TANK:
		HandleDeleteTank(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_CS_START_MOVE:
		HandleStartMove(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_CS_END_MOVE:
		HandleEndMove(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_CS_MOVING:
		HandleMoving(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_CS_SHOOT:
		HandleShoot(pGameEvent, senderId);
		break;
	default:
		__debugbreak();
		break;
	}
}

void GamePacket::SendSnapshot(UINT32 sessionId)
{
	UINT16 countObjects = g_objectManager.GetCountObjects();
	
	if (countObjects == 0) {
		const size_t PACKET_SIZE = sizeof(EGameEventCode) + sizeof(PACKET_SC_SNAPSHOT);
		BYTE pRawPacket[PACKET_SIZE] = { 0, };

		EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
		*pEvCode = GAME_EVENT_CODE_SC_SNAPSHOT;

		PACKET_SC_SNAPSHOT* pScSnapshot = (PACKET_SC_SNAPSHOT*)(pRawPacket + sizeof(EGameEventCode));
		pScSnapshot->countObjects = 0;
		g_pNetCore->SendMessageTo(sessionId, pRawPacket, PACKET_SIZE);
		return;
	}

	const size_t packet_size = sizeof(EGameEventCode) + sizeof(PACKET_SC_SNAPSHOT) + sizeof(PACKET_OBJECT_INFO) * countObjects;
	BYTE* pRawPacket = new BYTE[packet_size];
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	*pEvCode = GAME_EVENT_CODE_SC_SNAPSHOT;

	PACKET_SC_SNAPSHOT* pScSnapshot = (PACKET_SC_SNAPSHOT*)(pRawPacket + sizeof(EGameEventCode));
	pScSnapshot->countObjects = countObjects;

	PACKET_OBJECT_INFO* pObjInfo = (PACKET_OBJECT_INFO*)(pRawPacket + sizeof(EGameEventCode) + sizeof(PACKET_SC_SNAPSHOT));
	g_objectManager.CopySnapshot(pObjInfo);
	g_pNetCore->SendMessageTo(sessionId, pRawPacket, packet_size);
	delete[] pRawPacket;
}

void GamePacket::SendTankHit(UINT32 sessionId, UINT16 tankId, UINT16 projectileId)
{
	const size_t PACKET_SIZE = sizeof(EGameEventCode) + sizeof(PACKET_SC_TANK_HIT);
	BYTE pRawPacket[PACKET_SIZE];
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	*pEvCode = GAME_EVENT_CODE_SC_TANK_HIT;

	PACKET_SC_TANK_HIT* pScTankHit = (PACKET_SC_TANK_HIT*)(pRawPacket + sizeof(EGameEventCode));
	pScTankHit->projectileId = projectileId;
	pScTankHit->tankId = tankId;

	g_pNetCore->SendMessageTo(sessionId, pRawPacket, PACKET_SIZE);
}

void GamePacket::BroadcastTankHit(UINT16 tankId, UINT16 projectileId)
{
	const size_t PACKET_SIZE = sizeof(EGameEventCode) + sizeof(PACKET_SC_TANK_HIT);
	BYTE pRawPacket[PACKET_SIZE];
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	*pEvCode = GAME_EVENT_CODE_SC_TANK_HIT;

	PACKET_SC_TANK_HIT* pScTankHit = (PACKET_SC_TANK_HIT*)(pRawPacket + sizeof(EGameEventCode));
	pScTankHit->projectileId = projectileId;
	pScTankHit->tankId = tankId;

	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

void GamePacket::BroadcastCreateObstacle(UINT16 obstacleId, Transform* pTransform)
{
	const size_t PACKET_SIZE = sizeof(EGameEventCode) + sizeof(PACKET_SC_CREATE_OBSTACLE);
	BYTE pRawPacket[PACKET_SIZE];
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	*pEvCode = GAME_EVENT_CODE_SC_CREATE_OBSTACLE;
	PACKET_SC_CREATE_OBSTACLE* pScCreateObstacle = (PACKET_SC_CREATE_OBSTACLE*)(pRawPacket + sizeof(EGameEventCode));

	pScCreateObstacle->obstacleId = obstacleId;
	memcpy(&pScCreateObstacle->transform, pTransform, sizeof(Transform));

	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

void GamePacket::BroadcastDeleteObstacle(UINT16 obstacleId, UINT32 shooterId)
{
	const size_t PACKET_SIZE = sizeof(EGameEventCode) + sizeof(PACKET_SC_DELETE_OBSTACLE);
	BYTE pRawPacket[PACKET_SIZE];
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	*pEvCode = GAME_EVENT_CODE_SC_DELETE_OBSTACLE;

	PACKET_SC_DELETE_OBSTACLE* pScDeleteObstacle = (PACKET_SC_DELETE_OBSTACLE*)(pRawPacket + sizeof(EGameEventCode));

	pScDeleteObstacle->obstacleId = obstacleId;
	pScDeleteObstacle->shooterId = shooterId;

	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

void GamePacket::HandleCreateTank(BYTE* pGameEvent, UINT32 senderId)
{
	bool isValid = ValidateCreateTank(pGameEvent, senderId);
	if (!isValid) {
		printf("Invalid CreateTank packet from %u\n", senderId);
		return;
	}

	// hmm... the position of tank is determined by server.
	// PACKET_CS_CREATE_TANK* pCsPacketCreateTank = (PACKET_CS_CREATE_TANK*)(pGameEvent + sizeof(EGameEventCode));


	Tank* pTank = g_objectManager.CreateTank(senderId);

	const size_t PACKET_SIZE = sizeof(EGameEventCode) + sizeof(PACKET_SC_CREATE_TANK);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };

	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	*pEvCode = GAME_EVENT_CODE_SC_CREATE_TANK;
	PACKET_SC_CREATE_TANK* pSCCreateTank = (PACKET_SC_CREATE_TANK*)(pRawPacket + sizeof(EGameEventCode));
	pSCCreateTank->objectId = pTank->GetID();
	pSCCreateTank->ownerId = senderId;
	// 
	memcpy(&pSCCreateTank->transform, pTank->GetTransformPtr(), sizeof(Transform));

	printf("CreateTank: owner=%u, tankId=%u\n", senderId, pSCCreateTank->objectId);

	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

BOOL GamePacket::ValidateCreateTank(BYTE* pGameEvent, UINT32 senderId)
{
	Player* pPlayer = g_playerManager.GetPlayer(senderId);
	UINT32 tankId = pPlayer->GetTankId();
	if (tankId == 0) {
		return true;
	}
	return false;
}

void GamePacket::HandleDeleteTank(BYTE* pGameEvent, UINT32 senderId)
{
	bool isValid = ValidateDeleteTank(pGameEvent, senderId);
	if (!isValid) {
		printf("Invalid DeleteTank packet from %u\n", senderId);
		return;
	}

	PACKET_CS_DELETE_TANK* pCsDeleteTank = (PACKET_CS_DELETE_TANK*)(pGameEvent + sizeof(EGameEventCode));
	g_objectManager.RemoveTank(pCsDeleteTank->objectId, senderId);
	
	const size_t PACKET_SIZE = sizeof(PACKET_SC_DELETE_TANK) + sizeof(EGameEventCode);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };
	EGameEventCode* pGameEvCode = (EGameEventCode*)pRawPacket;
	*pGameEvCode = GAME_EVENT_CODE_SC_DELETE_TANK;

	PACKET_SC_DELETE_TANK* pScDeleteTank = (PACKET_SC_DELETE_TANK*)(pRawPacket + sizeof(EGameEventCode));
	pScDeleteTank->objectId = pCsDeleteTank->objectId;

	printf("DeleteTank: owner=%u, tankId=%u\n", senderId, pScDeleteTank->objectId);

	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

BOOL GamePacket::ValidateDeleteTank(BYTE* pGameEvent, UINT32 senderId)
{
	return true;
}

void GamePacket::HandleStartMove(BYTE* pGameEvent, UINT32 senderId)
{
	bool isValid = ValidateStartMove(pGameEvent, senderId);
	if (!isValid) {
		return;
	}

	PACKET_CS_START_MOVE* pCsStartMove = (PACKET_CS_START_MOVE*)(pGameEvent + sizeof(EGameEventCode));
	
	g_objectManager.UpdateTankTransform(senderId, &pCsStartMove->transform);
	
	Tank* pTank = g_objectManager.GetTank(senderId);
	if (pTank == nullptr) {
		return;
	}

	const size_t PACKET_SIZE = sizeof(PACKET_SC_START_MOVE) + sizeof(EGameEventCode);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };
	EGameEventCode* pGameEvCode = (EGameEventCode*)pRawPacket;
	*pGameEvCode = GAME_EVENT_CODE_SC_START_MOVE;

	PACKET_SC_START_MOVE* pScStartMove = (PACKET_SC_START_MOVE*)(pRawPacket + sizeof(EGameEventCode));
	pScStartMove->objectId = pTank->GetID();
	memcpy(&pScStartMove->transform, &pCsStartMove->transform, sizeof(Transform));

	printf("StartMove: owner=%u\n", senderId);
	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

BOOL GamePacket::ValidateStartMove(BYTE* pGameEvent, UINT32 senderId)
{
	return true;
}

void GamePacket::HandleEndMove(BYTE* pGameEvent, UINT32 senderId)
{
	bool isValid = ValidateEndMove(pGameEvent, senderId);
	if (!isValid) {
		return;
	}


	PACKET_CS_END_MOVE* pCsEndMove = (PACKET_CS_END_MOVE*)(pGameEvent + sizeof(EGameEventCode));
	g_objectManager.UpdateTankTransform(senderId, &pCsEndMove->transform);

	printf("EndMove: owner=%u\n", senderId);

	Tank* pTank = g_objectManager.GetTank(senderId);
	if (pTank == nullptr) {
		return;
	}

	const size_t PACKET_SIZE = sizeof(PACKET_SC_END_MOVE) + sizeof(EGameEventCode);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };
	EGameEventCode* pGameEvCode = (EGameEventCode*)pRawPacket;
	*pGameEvCode = GAME_EVENT_CODE_SC_END_MOVE;

	PACKET_SC_END_MOVE* pScEndMove = (PACKET_SC_END_MOVE*)(pRawPacket + sizeof(EGameEventCode));
	pScEndMove->objectId = pTank->GetID();
	memcpy(&pScEndMove->transform, &pCsEndMove->transform, sizeof(Transform));

	printf("EndMove: owner=%u\n", senderId);
	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

BOOL GamePacket::ValidateEndMove(BYTE* pGameEvent, UINT32 senderId)
{
	return true;
}

void GamePacket::HandleMoving(BYTE* pGameEvent, UINT32 senderId)
{
	bool isValid = ValidateMoving(pGameEvent, senderId);
	if (!isValid) {
		return;
	}

	PACKET_CS_MOVING* pCsMoving = (PACKET_CS_MOVING*)(pGameEvent + sizeof(EGameEventCode));
	g_objectManager.UpdateTankTransform(senderId, &pCsMoving->transform); // return failure

	Tank* pTank = g_objectManager.GetTank(senderId);
	if (pTank == nullptr) {
		return;
	}
	const size_t PACKET_SIZE = sizeof(PACKET_SC_MOVING) + sizeof(EGameEventCode);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };
	EGameEventCode* pGameEvCode = (EGameEventCode*)pRawPacket;
	*pGameEvCode = GAME_EVENT_CODE_SC_MOVING;

	PACKET_SC_MOVING* pScMoving = (PACKET_SC_MOVING*)(pRawPacket + sizeof(EGameEventCode));
	pScMoving->objectId = pTank->GetID();
	memcpy(&pScMoving->transform, &pCsMoving->transform, sizeof(Transform));

	printf("Moving: owner=%u\n", senderId);

	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

BOOL GamePacket::ValidateMoving(BYTE* pGameEvent, UINT32 senderId)
{
	return true;
}

void GamePacket::HandleShoot(BYTE* pGameEvent, UINT32 senderId)
{
	bool isValid = ValidateShoot(pGameEvent, senderId);
	if (!isValid) {
		printf("Invalid Shoot packet from %u\n", senderId);
		return;
	}

	PACKET_CS_SHOOT* pCsShoot = (PACKET_CS_SHOOT*)(pGameEvent + sizeof(EGameEventCode));


	Projectile* pProjectile = g_objectManager.CreateProjectile(senderId, &pCsShoot->transform);

	const size_t PACKET_SIZE = sizeof(EGameEventCode) + sizeof(PACKET_SC_SHOOT);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };

	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	*pEvCode = GAME_EVENT_CODE_SC_SHOOT;
	PACKET_SC_SHOOT* pScShoot= (PACKET_SC_SHOOT*)(pRawPacket + sizeof(EGameEventCode));
	pScShoot->objectId = pProjectile->GetID();
	pScShoot->ownerId = senderId;
	
	memcpy(&pScShoot->transform, pProjectile->GetTransformPtr(), sizeof(Transform));
	printf("Shoot by: owner=%u, projectileId=%u\n", senderId, pScShoot->objectId);

	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

BOOL GamePacket::ValidateShoot(BYTE* pGameEvent, UINT32 senderId)
{
	return true;
}
