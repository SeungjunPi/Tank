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

void GamePacket::BroadcastDeleteTank(UINT16 tankId)
{
	const size_t PACKET_SIZE = sizeof(EGameEventCode) + sizeof(PACKET_SC_DELETE_TANK);
	BYTE pRawPacket[PACKET_SIZE];

	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	*pEvCode = GAME_EVENT_CODE_SC_DELETE_TANK;
	PACKET_SC_DELETE_TANK* pScDeleteTank = (PACKET_SC_DELETE_TANK*)(pRawPacket + sizeof(EGameEventCode));

	pScDeleteTank->objectId = tankId;
	
	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

void GamePacket::BroadcastTankHit(UINT16 tankId, UINT16 projectileId, UINT32 shooterId, UINT32 targetId)
{
	const size_t PACKET_SIZE = sizeof(EGameEventCode) + sizeof(PACKET_SC_TANK_HIT);
	BYTE pRawPacket[PACKET_SIZE];
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	*pEvCode = GAME_EVENT_CODE_SC_TANK_HIT;

	PACKET_SC_TANK_HIT* pScTankHit = (PACKET_SC_TANK_HIT*)(pRawPacket + sizeof(EGameEventCode));
	pScTankHit->projectileId = projectileId;
	pScTankHit->tankId = tankId;
	pScTankHit->shooter = shooterId;
	pScTankHit->target = targetId;

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

void GamePacket::BroadcastRespawnTank(UINT16 tankId)
{
	const size_t PACKET_SIZE = sizeof(EGameEventCode) + sizeof(PACKET_SC_RESPAWN_TANK);
	BYTE pRawPacket[PACKET_SIZE];
	
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	*pEvCode = GAME_EVENT_CODE_SC_RESPAWN_TANK;

	PACKET_SC_RESPAWN_TANK* pScRespawnTank = (PACKET_SC_RESPAWN_TANK*)(pRawPacket + sizeof(EGameEventCode));

	pScRespawnTank->tankId = tankId;
	pScRespawnTank->position = { 0, };

	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

void GamePacket::HandleStartMove(BYTE* pGameEvent, UINT32 senderId)
{
	bool isValid = ValidateStartMove(pGameEvent, senderId);
	if (!isValid) {
		return;
	}

	PACKET_CS_START_MOVE* pCsStartMove = (PACKET_CS_START_MOVE*)(pGameEvent + sizeof(EGameEventCode));
	
	Tank* pTank = g_objectManager.GetTankByOwnerId(senderId);
	if (pTank == nullptr) {
		// Hit 등으로 인해 서버에서 먼저 사라졌을 수도 있다.
		return;
	}

	if (pCsStartMove->movementFlag & FLAG_MOVE_FORWARD) {
		g_objectManager.StartTankMove(senderId, EMOVEMENT::FORWARD);
	}

	if (pCsStartMove->movementFlag & FLAG_MOVE_BACKWARD) {
		g_objectManager.StartTankMove(senderId, EMOVEMENT::BACKWARD);
	}

	if (pCsStartMove->movementFlag & FLAG_ROTATE_LEFT) {
		g_objectManager.StartTankRotate(senderId, EROTATION::LEFT);
	}

	if (pCsStartMove->movementFlag & FLAG_ROTATE_RIGHT) {
		g_objectManager.StartTankRotate(senderId, EROTATION::RIGHT);
	}

	const size_t PACKET_SIZE = sizeof(PACKET_SC_START_MOVE) + sizeof(EGameEventCode);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };
	EGameEventCode* pGameEvCode = (EGameEventCode*)pRawPacket;
	*pGameEvCode = GAME_EVENT_CODE_SC_START_MOVE;

	PACKET_SC_START_MOVE* pScStartMove = (PACKET_SC_START_MOVE*)(pRawPacket + sizeof(EGameEventCode));
	pScStartMove->objectId = pTank->GetID();
	pScStartMove->movementFlag = pCsStartMove->movementFlag;
	memcpy(&pScStartMove->transform, pTank->GetTransformPtr(), sizeof(Transform));

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
	
	printf("EndMove: owner=%u\n", senderId);

	Tank* pTank = g_objectManager.GetTankByOwnerId(senderId);
	if (pTank == nullptr) {
		return;
	}

	if (pCsEndMove->movementFlag & FLAG_MOVE_FORWARD) {
		g_objectManager.EndTankMove(senderId, EMOVEMENT::FORWARD);
	}

	if (pCsEndMove->movementFlag & FLAG_MOVE_BACKWARD) {
		g_objectManager.EndTankMove(senderId, EMOVEMENT::BACKWARD);
	}

	if (pCsEndMove->movementFlag & FLAG_ROTATE_LEFT) {
		g_objectManager.EndTankRotate(senderId, EROTATION::LEFT);
	}

	if (pCsEndMove->movementFlag & FLAG_ROTATE_RIGHT) {
		g_objectManager.EndTankRotate(senderId, EROTATION::RIGHT);
	}

	BOOL isChanged = pTank->UpdateTransformIfValid(&pCsEndMove->transform);

	const size_t PACKET_SIZE = sizeof(PACKET_SC_END_MOVE) + sizeof(EGameEventCode);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };
	EGameEventCode* pGameEvCode = (EGameEventCode*)pRawPacket;
	*pGameEvCode = GAME_EVENT_CODE_SC_END_MOVE;

	PACKET_SC_END_MOVE* pScEndMove = (PACKET_SC_END_MOVE*)(pRawPacket + sizeof(EGameEventCode));
	pScEndMove->objectId = pTank->GetID();
	memcpy(&pScEndMove->transform, pTank->GetTransformPtr(), sizeof(Transform));

	pScEndMove->movementFlag = pCsEndMove->movementFlag;

	if (isChanged) {
		// 유저의 위치 정보를 그대로 사용하므로, 해당 유저에게는 재전송할 필요 없음
		GameServer::BroadcastExcept(pRawPacket, PACKET_SIZE, senderId);
	}
	else {
		// 서버의 위치 정보를 사용하므로, 해당 유저에게도 보내줘야 함
		GameServer::Broadcast(pRawPacket, PACKET_SIZE);
	}
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
	
	Tank* pTank = g_objectManager.GetTankByOwnerId(senderId);
	if (pTank == nullptr) {
		return;
	}

	BOOL isChanged = pTank->UpdateTransformIfValid(&pCsMoving->transform);

	const size_t PACKET_SIZE = sizeof(PACKET_SC_MOVING) + sizeof(EGameEventCode);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };
	EGameEventCode* pGameEvCode = (EGameEventCode*)pRawPacket;
	*pGameEvCode = GAME_EVENT_CODE_SC_MOVING;

	PACKET_SC_MOVING* pScMoving = (PACKET_SC_MOVING*)(pRawPacket + sizeof(EGameEventCode));
	pScMoving->objectId = pTank->GetID();
	memcpy(&pScMoving->transform, &pCsMoving->transform, sizeof(Transform));

	if (isChanged) {
		printf("Moving, accepted: owner=%u\n", senderId);
		GameServer::BroadcastExcept(pRawPacket, PACKET_SIZE, senderId);
	}
	else {
		printf("Moving, rejected: owner=%u\n", senderId);
		GameServer::Broadcast(pRawPacket, PACKET_SIZE);
	}
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
