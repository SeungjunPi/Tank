#include "ClientPacketHandler.h"
#include "Game.h"
#include "Global.h"
#include "AllocObjectManager.h"
#include "Player.h"
#include "INetCore.h"

void ClientPacketHandler::RegisterCallbacks()
{
	PacketHandler::RegisterCSSendLogin(SendLogin);
	PacketHandler::RegisterCSSendStartMove(SendStartMove);
	PacketHandler::RegisterCSSendEndMove(SendEndMove);
	PacketHandler::RegisterCSSendMoving(SendMoving);
	PacketHandler::RegisterCSSendFireMachineGun(SendFireMachineGun);

	PacketHandler::RegisterSCLoginCallback(OnLogin);
	PacketHandler::RegisterSCCreateTankCallback(OnCreateTank);
	PacketHandler::RegisterSCDeleteTankCallback(OnDeleteTank);
	PacketHandler::RegisterSCStartMoveCallback(OnStartMove);
	PacketHandler::RegisterSCEndMoveCallback(OnEndMove);
	PacketHandler::RegisterSCMovingCallback(OnMoving);
	PacketHandler::RegisterSCSnapshotCallback(OnSnapshot);
	PacketHandler::RegisterSCFireMachineGunCallback(OnFireMachineGun);
	PacketHandler::RegisterSCObjectHitCallback(OnObjectHit);
	PacketHandler::RegisterSCRespawnTankCallback(OnRespawnTank);
	PacketHandler::RegisterSCMachineGunHitCallback(OnMachineGunHit);
}

void ClientPacketHandler::OnLogin(const PACKET_SC_LOGIN* login, UINT32 senderID)
{
	if (login->result == FALSE) {
		// End game.
		Game::Shutdown();
		return;
	}
	g_pPlayer->OnSuccessLogin(login->userDBIndex, { login->hitCount , login->killCount , login->deathCount });
}

void ClientPacketHandler::OnCreateTank(const PACKET_SC_CREATE_TANK* createTank, UINT32 senderID)
{
	Tank* pTank = g_objectManager.CreateTank(createTank->objectId, createTank->ownerId);
	if (createTank->ownerId == g_pPlayer->GetUserID()) {
		g_pPlayer->SetTank(pTank);
		g_isTankCreateRequest = false;
	}
}

void ClientPacketHandler::OnDeleteTank(const PACKET_SC_DELETE_TANK* deleteTank, UINT32 senderID)
{
	if (deleteTank->objectId.equals(g_pPlayer->GetTankID())) {
		g_pPlayer->ClearTank();
	}
	g_objectManager.RemoveObject(deleteTank->objectId);
}

void ClientPacketHandler::OnStartMove(const PACKET_SC_START_MOVE* startMove, UINT32 senderID)
{
	if (startMove->objectId.equals(g_pPlayer->GetTankID())) {
		// hmm..
	}
	else {
		g_objectManager.SetObjectInputStateByServer(startMove->objectId, startMove->inputState);
		g_objectManager.UpdateObjectTransformFromServer(startMove->objectId, &startMove->transform);
	}
}

void ClientPacketHandler::OnEndMove(const PACKET_SC_END_MOVE* endMove, UINT32 senderID)
{
	if (endMove->objectId.equals(g_pPlayer->GetTankID())) {
		// overrided..
	}

	g_objectManager.SetObjectInputStateByServer(endMove->objectId, PLAYER_INPUT_NONE);
	g_objectManager.UpdateObjectTransformFromServer(endMove->objectId, &endMove->transform);

}

void ClientPacketHandler::OnMoving(const PACKET_SC_MOVING* moving, UINT32 senderID)
{
	if (moving->objectId.equals(g_pPlayer->GetTankID())) {
		// update if transforms are diffrent
	}

	g_objectManager.SetObjectInputStateByServer(moving->objectId, moving->inputState);
	g_objectManager.UpdateObjectTransformFromServer(moving->objectId, &moving->transform);
}

void ClientPacketHandler::OnSnapshot(const PACKET_SC_SNAPSHOT* snapshot, UINT32 senderID)
{
	if (snapshot->countObjects == 0) {
		// Todo: set start flag
		return;
	}

	PACKET_OBJECT_INFO* pObjInfo = (PACKET_OBJECT_INFO*)(snapshot + 1);
	for (UINT16 i = 0; i < snapshot->countObjects; ++i) {
		switch (pObjInfo->kind) {
		case GAME_OBJECT_TYPE_TANK:
		{
			Tank* pTank = g_objectManager.CreateTank(pObjInfo->objectId, pObjInfo->ownerId);
			pTank->SetTransformByServer(&pObjInfo->transform);
			break;
		}
		case GAME_OBJECT_TYPE_PROJECTILE:
		{
			Projectile* pProjectile = g_objectManager.CreateProjectile(pObjInfo->objectId, &pObjInfo->transform, pObjInfo->ownerId);
			pProjectile->SetTransformByServer(&pObjInfo->transform);
			break;
		}
		case GAME_OBJECT_TYPE_OBSTACLE:
			//g_objectManager.CreateObstacle(pObjInfo->objectId);
			break;
		default:
			__debugbreak();
		}
		++pObjInfo;
	}
}

void ClientPacketHandler::OnFireMachineGun(const PACKET_SC_FIRE_MACHINE_GUN* fireMachineGun, UINT32 senderID)
{
	g_objectManager.CreateProjectile(fireMachineGun->objectId, &fireMachineGun->transform, fireMachineGun->ownerId);
}

void ClientPacketHandler::OnObjectHit(const PACKET_SC_OBJECT_HIT* objectHit, UINT32 senderID)
{
	ObjectID oneID = objectHit->oneID;
	ObjectID anotherID = objectHit->anotherID;

	GameObject* pOne = g_objectManager.GetObjectPtrOrNull(oneID);
	GameObject* pAnother = g_objectManager.GetObjectPtrOrNull(anotherID);

	pOne->OnHitServer(g_currentGameTick, pAnother);
	pAnother->OnHitServer(g_currentGameTick, pOne);
}

void ClientPacketHandler::OnRespawnTank(const PACKET_SC_RESPAWN_TANK* respawnTank, UINT32 senderID)
{
	GameObject* pTank = g_objectManager.GetObjectPtrOrNull(respawnTank->tankId);
	pTank->Respawn();
}

void ClientPacketHandler::OnMachineGunHit(const PACKET_SC_MACHINE_GUN_HIT* machineGunHit, UINT32 senderID)
{
	if (machineGunHit->shooter == g_pPlayer->GetUserID()) {
		
	}

	
	
	machineGunHit->victim;
}

void ClientPacketHandler::SendLogin(const std::wstring& wID, const std::wstring& wPw, SessionID sessionID)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_LOGIN) + sizeof(ENetworkMessageType);

	BYTE pRawPacket[contentsMsgSize] = { 0, };

	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	PACKET_CS_LOGIN* pLogin = (PACKET_CS_LOGIN*)(pRawPacket + sizeof(ENetworkMessageType));

	const WCHAR* rawID = wID.c_str();
	const WCHAR* rawPW = wPw.c_str();
	memcpy(&pLogin->id, rawID, sizeof(WCHAR) * (wID.length() + 1));
	memcpy(&pLogin->pw, rawPW, sizeof(WCHAR) * (wPw.length() + 1));

	g_pNetCore->SendMessageTo(sessionID, pRawPacket, contentsMsgSize);
}

void ClientPacketHandler::SendStartMove(const Transform* pTankTransform, PlayerInputState inputState, SessionID sessionID)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_START_MOVE) + sizeof(ENetworkMessageType);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	PACKET_CS_START_MOVE* pContentsMsgBody = (PACKET_CS_START_MOVE*)(pRawPacket + sizeof(ENetworkMessageType));

	*pEvCode = GAME_MESSAGE_TYPE_CS_START_MOVE;
	pContentsMsgBody->inputState = inputState;
	g_pNetCore->SendMessageTo(sessionID, pRawPacket, contentsMsgSize);
}

void ClientPacketHandler::SendEndMove(const Transform* pTankTransform, SessionID sessionID)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_END_MOVE) + sizeof(ENetworkMessageType);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	PACKET_CS_END_MOVE* pContentsMsgBody = (PACKET_CS_END_MOVE*)(pRawPacket + sizeof(ENetworkMessageType));

	*pEvCode = GAME_MESSAGE_TYPE_CS_END_MOVE;
	memcpy(&pContentsMsgBody->transform, pTankTransform, sizeof(Transform));
	g_pNetCore->SendMessageTo(sessionID, pRawPacket, contentsMsgSize);
}

void ClientPacketHandler::SendMoving(const Transform* pTankTransform, PlayerInputState moveFlag, SessionID sessionID)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_MOVING) + sizeof(ENetworkMessageType);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	PACKET_CS_MOVING* pContentsMsgBody = (PACKET_CS_MOVING*)(pRawPacket + sizeof(ENetworkMessageType));

	*pEvCode = GAME_MESSAGE_TYPE_CS_MOVING;
	memcpy(&pContentsMsgBody->transform, pTankTransform, sizeof(Transform));
	pContentsMsgBody->inputState = moveFlag;
	g_pNetCore->SendMessageTo(sessionID, pRawPacket, contentsMsgSize);
}

void ClientPacketHandler::SendFireMachineGun(const Transform* pTurretTransform, SessionID sessionID)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_FIRE_MACHINE_GUN) + sizeof(ENetworkMessageType);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	PACKET_CS_FIRE_MACHINE_GUN* pContentsMsgBody = (PACKET_CS_FIRE_MACHINE_GUN*)(pRawPacket + sizeof(ENetworkMessageType));

	*pEvCode = GAME_MESSAGE_TYPE_CS_FIRE_MACHINE_GUN;
	memcpy(&pContentsMsgBody->transform, pTurretTransform, sizeof(Transform));
	g_pNetCore->SendMessageTo(sessionID, pRawPacket, contentsMsgSize);
}
