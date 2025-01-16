#include "GameEvent.h"
#include "GameServer.h"
#include "Global.h"
#include "NetCore.h"
#include "Tank.h"
#include "Projectile.h"
#include "JunDB.h"
#include "ObjectManager.h"
#include "PlayerManager.h"



BOOL GamePacket::Validate(BYTE* pGameEvent, SessionID senderId)
{
	return true;
}

void GamePacket::HandlePacket(BYTE* pGameEvent, SessionID senderId)
{
	EGameEventCode eventCode = *(EGameEventCode*)pGameEvent;
	switch (eventCode) {
	case GAME_EVENT_CODE_CS_LOGIN:
		HandleLogin(pGameEvent, senderId);
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

void GamePacket::SendSnapshot(SessionID sessionId)
{
	UINT32 countObjects = g_objectManager.GetCountObjects();
	
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
	BYTE* pRawPacket = DNew BYTE[packet_size];
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	*pEvCode = GAME_EVENT_CODE_SC_SNAPSHOT;

	PACKET_SC_SNAPSHOT* pScSnapshot = (PACKET_SC_SNAPSHOT*)(pRawPacket + sizeof(EGameEventCode));
	pScSnapshot->countObjects = countObjects;

	PACKET_OBJECT_INFO* pObjInfo = (PACKET_OBJECT_INFO*)(pRawPacket + sizeof(EGameEventCode) + sizeof(PACKET_SC_SNAPSHOT));
	g_objectManager.CopySnapshot(pObjInfo);
	g_pNetCore->SendMessageTo(sessionId, pRawPacket, packet_size);
	delete[] pRawPacket;
}

void GamePacket::SendTankHit(SessionID sessionId, ObjectID tankId, ObjectID projectileId)
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

void GamePacket::BroadcastDeleteTank(ObjectID tankId)
{
	const size_t PACKET_SIZE = sizeof(EGameEventCode) + sizeof(PACKET_SC_DELETE_TANK);
	BYTE pRawPacket[PACKET_SIZE];

	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	*pEvCode = GAME_EVENT_CODE_SC_DELETE_TANK;
	PACKET_SC_DELETE_TANK* pScDeleteTank = (PACKET_SC_DELETE_TANK*)(pRawPacket + sizeof(EGameEventCode));

	pScDeleteTank->objectId = tankId;
	
	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

void GamePacket::BroadcastTankHit(ObjectID tankId, ObjectID projectileId, UserDBIndex shooterId, UserDBIndex targetId)
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

void GamePacket::BroadcastCreateObstacle(ObjectID obstacleId, Transform* pTransform)
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

void GamePacket::BroadcastDeleteObstacle(ObjectID obstacleId, UserDBIndex shooterId)
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

void GamePacket::BroadcastRespawnTank(ObjectID tankId)
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

void GamePacket::HandleLogin(BYTE* pGameEvent, SessionID senderId)
{
	bool isValid = ValidateLogin(pGameEvent, senderId);
	if (!isValid) {
		return;
	}

	PACKET_CS_LOGIN* pCsLogin = (PACKET_CS_LOGIN*)(pGameEvent + sizeof(EGameEventCode));
	g_pJunDB->ValidateUserInfo(pCsLogin->id, pCsLogin->pw, senderId);
}

BOOL GamePacket::ValidateLogin(BYTE* pGameEvent, SessionID senderId)
{
	return TRUE;
}

void GamePacket::HandleStartMove(BYTE* pGameEvent, SessionID senderId)
{
	bool isValid = ValidateStartMove(pGameEvent, senderId);
	if (!isValid) {
		return;
	}

	PACKET_CS_START_MOVE* pCsStartMove = (PACKET_CS_START_MOVE*)(pGameEvent + sizeof(EGameEventCode));
	Player* pPlayer = g_playerManager.GetPlayerBySessionID(senderId);
	if (pPlayer == nullptr) {
		__debugbreak();
	}
	UserDBIndex userIndex = pPlayer->GetUserIndex();
	
	Tank* pTank = g_objectManager.GetTankByOwnerId(userIndex);
	if (pTank == nullptr) {
		// Hit 등으로 인해 서버에서 먼저 사라졌을 수도 있다.
		return;
	}

	if (pCsStartMove->movementFlag & FLAG_MOVE_FORWARD) {
		g_objectManager.StartTankMove(userIndex, EMOVEMENT::FORWARD);
	}

	if (pCsStartMove->movementFlag & FLAG_MOVE_BACKWARD) {
		g_objectManager.StartTankMove(userIndex, EMOVEMENT::BACKWARD);
	}

	if (pCsStartMove->movementFlag & FLAG_ROTATE_LEFT) {
		g_objectManager.StartTankRotate(userIndex, EROTATION::LEFT);
	}

	if (pCsStartMove->movementFlag & FLAG_ROTATE_RIGHT) {
		g_objectManager.StartTankRotate(userIndex, EROTATION::RIGHT);
	}

	const size_t PACKET_SIZE = sizeof(PACKET_SC_START_MOVE) + sizeof(EGameEventCode);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };
	EGameEventCode* pGameEvCode = (EGameEventCode*)pRawPacket;
	*pGameEvCode = GAME_EVENT_CODE_SC_START_MOVE;

	PACKET_SC_START_MOVE* pScStartMove = (PACKET_SC_START_MOVE*)(pRawPacket + sizeof(EGameEventCode));
	pScStartMove->objectId = pTank->GetID();
	pScStartMove->movementFlag = pCsStartMove->movementFlag;
	memcpy(&pScStartMove->transform, pTank->GetTransformPtr(), sizeof(Transform));

	printf("StartMove: owner=%u\n", userIndex);
	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

BOOL GamePacket::ValidateStartMove(BYTE* pGameEvent, SessionID senderId)
{
	return true;
}

void GamePacket::HandleEndMove(BYTE* pGameEvent, SessionID senderId)
{
	bool isValid = ValidateEndMove(pGameEvent, senderId);
	if (!isValid) {
		return;
	}

	PACKET_CS_END_MOVE* pCsEndMove = (PACKET_CS_END_MOVE*)(pGameEvent + sizeof(EGameEventCode));

	Player* pPlayer = g_playerManager.GetPlayerBySessionID(senderId);
	if (pPlayer == nullptr) {
		__debugbreak();
	}
	UserDBIndex userIndex = pPlayer->GetUserIndex();
	printf("EndMove: owner=%u\n", userIndex);

	Tank* pTank = g_objectManager.GetTankByOwnerId(userIndex);
	if (pTank == nullptr) {
		return;
	}

	if (pCsEndMove->movementFlag & FLAG_MOVE_FORWARD) {
		g_objectManager.EndTankMove(userIndex, EMOVEMENT::FORWARD);
	}

	if (pCsEndMove->movementFlag & FLAG_MOVE_BACKWARD) {
		g_objectManager.EndTankMove(userIndex, EMOVEMENT::BACKWARD);
	}

	if (pCsEndMove->movementFlag & FLAG_ROTATE_LEFT) {
		g_objectManager.EndTankRotate(userIndex, EROTATION::LEFT);
	}

	if (pCsEndMove->movementFlag & FLAG_ROTATE_RIGHT) {
		g_objectManager.EndTankRotate(userIndex, EROTATION::RIGHT);
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

BOOL GamePacket::ValidateEndMove(BYTE* pGameEvent, SessionID senderId)
{
	return true;
}

void GamePacket::HandleMoving(BYTE* pGameEvent, SessionID senderId)
{
	bool isValid = ValidateMoving(pGameEvent, senderId);
	if (!isValid) {
		return;
	}

	PACKET_CS_MOVING* pCsMoving = (PACKET_CS_MOVING*)(pGameEvent + sizeof(EGameEventCode));

	Player* pPlayer = g_playerManager.GetPlayerBySessionID(senderId);
	if (pPlayer == nullptr) {
		__debugbreak();
	}
	UserDBIndex userIndex = pPlayer->GetUserIndex();
	
	Tank* pTank = g_objectManager.GetTankByOwnerId(userIndex);
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
	if (pScMoving->objectId.type != GAME_OBJECT_TYPE_TANK) {
		__debugbreak();
	}

	if (isChanged) {
		printf("Moving, accepted: owner=%u\n", userIndex);
		GameServer::BroadcastExcept(pRawPacket, PACKET_SIZE, senderId);
	}
	else {
		printf("Moving, rejected: owner=%u\n", userIndex);
		GameServer::Broadcast(pRawPacket, PACKET_SIZE);
	}
}

BOOL GamePacket::ValidateMoving(BYTE* pGameEvent, SessionID senderId)
{
	return true;
}

void GamePacket::HandleShoot(BYTE* pGameEvent, SessionID senderId)
{
	bool isValid = ValidateShoot(pGameEvent, senderId);
	if (!isValid) {
		printf("Invalid Shoot packet from %u\n", senderId);
		return;
	}

	PACKET_CS_SHOOT* pCsShoot = (PACKET_CS_SHOOT*)(pGameEvent + sizeof(EGameEventCode));
	Player* pPlayer = g_playerManager.GetPlayerBySessionID(senderId);
	if (pPlayer == nullptr) {
		__debugbreak();
	}
	UserDBIndex userIndex = pPlayer->GetUserIndex();

	Projectile* pProjectile = g_objectManager.CreateProjectile(userIndex, &pCsShoot->transform);

	const size_t PACKET_SIZE = sizeof(EGameEventCode) + sizeof(PACKET_SC_SHOOT);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };

	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	*pEvCode = GAME_EVENT_CODE_SC_SHOOT;
	PACKET_SC_SHOOT* pScShoot= (PACKET_SC_SHOOT*)(pRawPacket + sizeof(EGameEventCode));
	pScShoot->objectId = pProjectile->GetID();
	pScShoot->ownerId = userIndex;
	
	memcpy(&pScShoot->transform, pProjectile->GetTransformPtr(), sizeof(Transform));
	printf("Shoot by: owner=%u, projectileId=%u\n", userIndex, pScShoot->objectId.key);

	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

BOOL GamePacket::ValidateShoot(BYTE* pGameEvent, SessionID senderId)
{
	return true;
}
