#include "ServerPacketHandler.h"

#include "GameServer.h"
#include "Global.h"
#include "ObjectManager.h"
#include "INetCore.h"
#include "JunDB.h"
#include "PlayerManager.h"
#include "Tank.h"
#include "Projectile.h"

void ServerPacketHandler::RegisterCallbacks()
{
	PacketHandler::RegisterCSLoginCallback(OnLogin);
	PacketHandler::RegisterCSStartMoveCallback(OnStartMove);
	PacketHandler::RegisterCSEndMoveCallback(OnEndMove);
	PacketHandler::RegisterCSMovingCallback(OnMoving);
	PacketHandler::RegisterCSFireMachineGunCallback(OnFireMachineGun);
}

void ServerPacketHandler::SendLogin(SessionID sessionId, BOOL result, UserDBIndex userDBIndex, int hitCount, int killCount, int deathCount)
{
	const size_t msgSize = sizeof(ENetworkMessageType) + sizeof(PACKET_SC_LOGIN);
	BYTE pRawMsg[msgSize] = { 0, };
	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawMsg;
	*pEvCode = GAME_MESSAGE_TYPE_SC_LOGIN;
	PACKET_SC_LOGIN* pScLogin = (PACKET_SC_LOGIN*)(pRawMsg + sizeof(ENetworkMessageType));
	pScLogin->result = true;
	pScLogin->userDBIndex = userDBIndex;
	pScLogin->hitCount = hitCount;
	pScLogin->killCount = killCount;
	pScLogin->deathCount = deathCount;
	g_pNetCore->SendMessageTo(sessionId, pRawMsg, msgSize);
}

void ServerPacketHandler::SendSnapshot(SessionID sessionId)
{
	UINT32 countObjects = g_objectManager.GetCountObjects();

	if (countObjects == 0) {
		const size_t PACKET_SIZE = sizeof(ENetworkMessageType) + sizeof(PACKET_SC_SNAPSHOT);
		BYTE pRawPacket[PACKET_SIZE] = { 0, };

		ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
		*pEvCode = GAME_MESSAGE_TYPE_SC_SNAPSHOT;

		PACKET_SC_SNAPSHOT* pScSnapshot = (PACKET_SC_SNAPSHOT*)(pRawPacket + sizeof(ENetworkMessageType));
		pScSnapshot->countObjects = 0;
		g_pNetCore->SendMessageTo(sessionId, pRawPacket, PACKET_SIZE);
		return;
	}

	const size_t packet_size = sizeof(ENetworkMessageType) + sizeof(PACKET_SC_SNAPSHOT) + sizeof(PACKET_OBJECT_INFO) * countObjects;
	BYTE* pRawPacket = DNew BYTE[packet_size];
	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	*pEvCode = GAME_MESSAGE_TYPE_SC_SNAPSHOT;

	PACKET_SC_SNAPSHOT* pScSnapshot = (PACKET_SC_SNAPSHOT*)(pRawPacket + sizeof(ENetworkMessageType));
	pScSnapshot->countObjects = countObjects;

	PACKET_OBJECT_INFO* pObjInfo = (PACKET_OBJECT_INFO*)(pRawPacket + sizeof(ENetworkMessageType) + sizeof(PACKET_SC_SNAPSHOT));
	g_objectManager.CopySnapshot(pObjInfo);
	g_pNetCore->SendMessageTo(sessionId, pRawPacket, packet_size);
	delete[] pRawPacket;
}

void ServerPacketHandler::BroadcastCreateTank(ObjectID tankId, UserDBIndex userIndex, const Transform* pTransform)
{
	const size_t PACKET_SIZE = sizeof(ENetworkMessageType) + sizeof(PACKET_SC_CREATE_TANK);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };

	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	*pEvCode = GAME_MESSAGE_TYPE_SC_CREATE_TANK;
	PACKET_SC_CREATE_TANK* pSCCreateTank = (PACKET_SC_CREATE_TANK*)(pRawPacket + sizeof(ENetworkMessageType));
	pSCCreateTank->objectId = tankId;
	pSCCreateTank->ownerId = userIndex;
	// 
	memcpy(&pSCCreateTank->transform, pTransform, sizeof(Transform));

	printf("CreateTank: owner=%u, tankId=%u\n", userIndex, pSCCreateTank->objectId.key);

	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

void ServerPacketHandler::BroadcastDeleteTank(ObjectID tankId)
{
	const size_t PACKET_SIZE = sizeof(ENetworkMessageType) + sizeof(PACKET_SC_DELETE_TANK);
	BYTE pRawPacket[PACKET_SIZE];

	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	*pEvCode = GAME_MESSAGE_TYPE_SC_DELETE_TANK;
	PACKET_SC_DELETE_TANK* pScDeleteTank = (PACKET_SC_DELETE_TANK*)(pRawPacket + sizeof(ENetworkMessageType));

	pScDeleteTank->objectId = tankId;

	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

void ServerPacketHandler::BroadcastHit(ObjectID oneID, ObjectID anotherID)
{
	const size_t PACKET_SIZE = sizeof(ENetworkMessageType) + sizeof(PACKET_SC_OBJECT_HIT);
	BYTE pRawPacket[PACKET_SIZE];

	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	*pEvCode = GAME_MESSAGE_TYPE_SC_OBJECT_HIT;

	PACKET_SC_OBJECT_HIT* pScObjectHit = (PACKET_SC_OBJECT_HIT*)(pRawPacket + sizeof(ENetworkMessageType));
	pScObjectHit->oneID = oneID;
	pScObjectHit->anotherID = anotherID;

	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

void ServerPacketHandler::BroadcastCreateObstacle(ObjectID obstacleId, Transform* pTransform)
{
	//const size_t PACKET_SIZE = sizeof(ENetworkMessageType) + sizeof(PACKET_SC_CREATE_OBSTACLE);
	//BYTE pRawPacket[PACKET_SIZE];
	//ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	//*pEvCode = GAME_EVENT_CODE_SC_CREATE_OBSTACLE;
	//PACKET_SC_CREATE_OBSTACLE* pScCreateObstacle = (PACKET_SC_CREATE_OBSTACLE*)(pRawPacket + sizeof(ENetworkMessageType));

	//pScCreateObstacle->obstacleId = obstacleId;
	//memcpy(&pScCreateObstacle->transform, pTransform, sizeof(Transform));

	//GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

void ServerPacketHandler::BroadcastDeleteObstacle(ObjectID obstacleId, UserDBIndex shooterId)
{
	//const size_t PACKET_SIZE = sizeof(ENetworkMessageType) + sizeof(PACKET_SC_DELETE_OBSTACLE);
	//BYTE pRawPacket[PACKET_SIZE];
	//ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	//*pEvCode = GAME_EVENT_CODE_SC_DELETE_OBSTACLE;

	//PACKET_SC_DELETE_OBSTACLE* pScDeleteObstacle = (PACKET_SC_DELETE_OBSTACLE*)(pRawPacket + sizeof(ENetworkMessageType));

	//pScDeleteObstacle->obstacleId = obstacleId;
	//pScDeleteObstacle->shooterId = shooterId;

	//GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

void ServerPacketHandler::BroadcastRespawnTank(ObjectID tankId)
{
	const size_t PACKET_SIZE = sizeof(ENetworkMessageType) + sizeof(PACKET_SC_RESPAWN_TANK);
	BYTE pRawPacket[PACKET_SIZE];

	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	*pEvCode = GAME_MESSAGE_TYPE_SC_RESPAWN_TANK;

	PACKET_SC_RESPAWN_TANK* pScRespawnTank = (PACKET_SC_RESPAWN_TANK*)(pRawPacket + sizeof(ENetworkMessageType));

	pScRespawnTank->tankId = tankId;
	pScRespawnTank->position = { 0, };

	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}

void ServerPacketHandler::OnLogin(const PACKET_CS_LOGIN* login, UINT32 senderID)
{
	g_pJunDB->ValidateUserInfo(login->id, login->pw, senderID);
}

void ServerPacketHandler::OnStartMove(const PACKET_CS_START_MOVE* startMove, UINT32 senderID)
{
	Player* pPlayer = g_playerManager.GetPlayerBySessionID(senderID);
	if (pPlayer == nullptr) {
		__debugbreak();
	}
	UserDBIndex userIndex = pPlayer->GetUserIndex();

	Tank* pTank = g_objectManager.GetTankByOwnerId(userIndex);
	if (pTank == nullptr) {
		// Hit 등으로 인해 서버에서 먼저 사라졌을 수도 있다.
		return;
	}

	pTank->AdvancePlayerInput(startMove->inputState);

	const size_t PACKET_SIZE = sizeof(PACKET_SC_START_MOVE) + sizeof(ENetworkMessageType);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };

	ENetworkMessageType* pGameEvCode = (ENetworkMessageType*)pRawPacket;
	*pGameEvCode = GAME_MESSAGE_TYPE_SC_START_MOVE;

	PACKET_SC_START_MOVE* pScStartMove = (PACKET_SC_START_MOVE*)(pRawPacket + sizeof(ENetworkMessageType));
	pScStartMove->objectId = pTank->GetID();
	pScStartMove->inputState = startMove->inputState;
	memcpy(&pScStartMove->transform, pTank->GetTransformPtr(), sizeof(Transform));

	// printf("StartMove: owner=%u\n", userIndex);
	GameServer::BroadcastExcept(pRawPacket, PACKET_SIZE, senderID);
}

void ServerPacketHandler::OnEndMove(const PACKET_CS_END_MOVE* endMove, UINT32 senderID)
{
	Player* pPlayer = g_playerManager.GetPlayerBySessionID(senderID);
	if (pPlayer == nullptr) {
		__debugbreak();
	}
	UserDBIndex userIndex = pPlayer->GetUserIndex();
	// printf("EndMove: owner=%u\n", userIndex);

	Tank* pTank = g_objectManager.GetTankByOwnerId(userIndex);
	if (pTank == nullptr) {
		return;
	}

	pTank->AdvancePlayerInput(PLAYER_INPUT_NONE);

	BOOL isChanged = pTank->UpdateTransformIfValid(&endMove->transform);

	const size_t PACKET_SIZE = sizeof(PACKET_SC_END_MOVE) + sizeof(ENetworkMessageType);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };
	ENetworkMessageType* pNetworkMessageType = (ENetworkMessageType*)pRawPacket;
	*pNetworkMessageType = GAME_MESSAGE_TYPE_SC_END_MOVE;

	PACKET_SC_END_MOVE* pScEndMove = (PACKET_SC_END_MOVE*)(pRawPacket + sizeof(ENetworkMessageType));
	pScEndMove->objectId = pTank->GetID();
	memcpy(&pScEndMove->transform, pTank->GetTransformPtr(), sizeof(Transform));

	if (isChanged) {
		// 유저의 위치 정보를 그대로 사용하므로, 해당 유저에게는 재전송할 필요 없음
		GameServer::BroadcastExcept(pRawPacket, PACKET_SIZE, senderID);
	}
	else {
		// 서버의 위치 정보를 사용하므로, 해당 유저에게도 보내줘야 함
		GameServer::Broadcast(pRawPacket, PACKET_SIZE);
	}
}

void ServerPacketHandler::OnMoving(const PACKET_CS_MOVING* moving, UINT32 senderID)
{
	Player* pPlayer = g_playerManager.GetPlayerBySessionID(senderID);
	if (pPlayer == nullptr) {
		// Disconnect와 Moving이 동시에 쌓여있다면, Disconnect가 먼저 처리되므로 이 경우가 생길 수 있음. 
		return;
	}
	UserDBIndex userIndex = pPlayer->GetUserIndex();

	Tank* pTank = g_objectManager.GetTankByOwnerId(userIndex);
	if (pTank == nullptr) {
		return;
	}

	pTank->AdvancePlayerInput(moving->inputState);
	BOOL isChanged = pTank->UpdateTransformIfValid(&moving->transform);

	const size_t PACKET_SIZE = sizeof(PACKET_SC_MOVING) + sizeof(ENetworkMessageType);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };
	ENetworkMessageType* pGameEvCode = (ENetworkMessageType*)pRawPacket;
	*pGameEvCode = GAME_MESSAGE_TYPE_SC_MOVING;

	PACKET_SC_MOVING* pScMoving = (PACKET_SC_MOVING*)(pRawPacket + sizeof(ENetworkMessageType));
	pScMoving->objectId = pTank->GetID();
	pScMoving->inputState = moving->inputState;

	if (pScMoving->objectId.type != GAME_OBJECT_TYPE_TANK) {
		__debugbreak();
	}

	if (isChanged) {
		// printf("Moving, accepted: owner=%u\n", userIndex);
		memcpy(&pScMoving->transform, &moving->transform, sizeof(Transform));
		GameServer::BroadcastExcept(pRawPacket, PACKET_SIZE, senderID);
	}
	else {
		// printf("Moving, rejected: owner=%u\n", userIndex);
		memcpy(&pScMoving->transform, pTank->GetTransformPtr(), sizeof(Transform));
		GameServer::Broadcast(pRawPacket, PACKET_SIZE);
	}
}

void ServerPacketHandler::OnFireMachineGun(const PACKET_CS_FIRE_MACHINE_GUN* fireMachineGun, UINT32 senderID)
{
	Player* pPlayer = g_playerManager.GetPlayerBySessionID(senderID);
	if (pPlayer == nullptr) {
		__debugbreak();
	}

	UserDBIndex userIndex = pPlayer->GetUserIndex();
	Projectile* pProjectile = g_objectManager.CreateProjectile(userIndex, &fireMachineGun->transform);

	const size_t PACKET_SIZE = sizeof(ENetworkMessageType) + sizeof(PACKET_SC_FIRE_MACHINE_GUN);
	BYTE pRawPacket[PACKET_SIZE] = { 0, };

	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	*pEvCode = GAME_MESSAGE_TYPE_SC_FIRE_MACHINE_GUN;
	PACKET_SC_FIRE_MACHINE_GUN* pScFireMachineGun = (PACKET_SC_FIRE_MACHINE_GUN*)(pRawPacket + sizeof(ENetworkMessageType));
	pScFireMachineGun->objectId = pProjectile->GetID();
	pScFireMachineGun->ownerId = userIndex;

	memcpy(&pScFireMachineGun->transform, pProjectile->GetTransformPtr(), sizeof(Transform));
	printf("Shoot by: owner=%u, projectileId=%u, at [%f, %f, %f]\n", userIndex, pScFireMachineGun->objectId.key, pScFireMachineGun->transform.Position.x, pScFireMachineGun->transform.Position.y, pScFireMachineGun->transform.Position.z);

	GameServer::Broadcast(pRawPacket, PACKET_SIZE);
}


