#include "DummyPacketHandler.h"
#include "DummyGlobal.h"
#include "Global.h"
#include "NetCore.h"
#include "Tank.h"
#include "AllocObjectManager.h"
#include "DummyGame.h"
#include "Player.h"
#include "DummyManager.h"
#include "Dummy.h"



void DummyPacketHandler::RegisterCallbacks()
{
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
}

void DummyPacketHandler::SendLogin(const std::wstring& wID, const std::wstring& wPw, SessionID sessionID)
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

void DummyPacketHandler::SendStartMove(const Transform* pTankTransform, PlayerInputState inputState, SessionID sessionID)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_START_MOVE) + sizeof(ENetworkMessageType);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	PACKET_CS_START_MOVE* pContentsMsgBody = (PACKET_CS_START_MOVE*)(pRawPacket + sizeof(ENetworkMessageType));

	*pEvCode = GAME_MESSAGE_TYPE_CS_START_MOVE;
	pContentsMsgBody->inputState = inputState;
	g_pNetCore->SendMessageTo(sessionID, pRawPacket, contentsMsgSize);
}

void DummyPacketHandler::SendEndMove(const Transform* pTankTransform, SessionID sessionID)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_END_MOVE) + sizeof(ENetworkMessageType);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	PACKET_CS_END_MOVE* pContentsMsgBody = (PACKET_CS_END_MOVE*)(pRawPacket + sizeof(ENetworkMessageType));

	*pEvCode = GAME_MESSAGE_TYPE_CS_END_MOVE;
	memcpy(&pContentsMsgBody->transform, pTankTransform, sizeof(Transform));
	g_pNetCore->SendMessageTo(sessionID, pRawPacket, contentsMsgSize);
}

void DummyPacketHandler::SendMoving(const Transform* pTankTransform, PlayerInputState inputState, SessionID sessionID)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_MOVING) + sizeof(ENetworkMessageType);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	PACKET_CS_MOVING* pContentsMsgBody = (PACKET_CS_MOVING*)(pRawPacket + sizeof(ENetworkMessageType));

	*pEvCode = GAME_MESSAGE_TYPE_CS_MOVING;
	memcpy(&pContentsMsgBody->transform, pTankTransform, sizeof(Transform));
	pContentsMsgBody->inputState = inputState;
	g_pNetCore->SendMessageTo(sessionID, pRawPacket, contentsMsgSize);
}

void DummyPacketHandler::SendFireMachineGun(const Transform* pTurretTransform, SessionID sessionID)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_FIRE_MACHINE_GUN) + sizeof(ENetworkMessageType);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	ENetworkMessageType* pEvCode = (ENetworkMessageType*)pRawPacket;
	PACKET_CS_FIRE_MACHINE_GUN* pContentsMsgBody = (PACKET_CS_FIRE_MACHINE_GUN*)(pRawPacket + sizeof(ENetworkMessageType));

	*pEvCode = GAME_MESSAGE_TYPE_CS_FIRE_MACHINE_GUN;
	memcpy(&pContentsMsgBody->transform, pTurretTransform, sizeof(Transform));
	g_pNetCore->SendMessageTo(sessionID, pRawPacket, contentsMsgSize);
}

void DummyPacketHandler::OnLogin(const PACKET_SC_LOGIN* login, UINT32 sessionID)
{
	if (login->result == FALSE) {
		// End game.
		g_pDummyManager->RemoveDummy(sessionID);
		return;
	}

	Dummy* pDummy = g_pDummyManager->GetDummyBySessionID(sessionID);
	// Find dummy and call
	pDummy->OnSuccessLogin(login->userDBIndex, { login->hitCount , login->killCount , login->deathCount });
}

void DummyPacketHandler::OnCreateTank(const PACKET_SC_CREATE_TANK* createTank, UINT32 sessionID)
{
	if (!g_pDummyManager->IsMaster(sessionID)) {
		return;
	}

	Tank* pTank = g_objectManager.CreateTank(createTank->objectId, createTank->ownerId);

	Dummy* pDummy = g_pDummyManager->GetDummyByUserID(createTank->ownerId);
	if (pDummy != nullptr) {
		pDummy->SetTank(pTank);
	}
	else {
		//	printf("Here????????\n");
	}
}

void DummyPacketHandler::OnDeleteTank(const PACKET_SC_DELETE_TANK* deleteTank, UINT32 sessionID)
{
	if (!g_pDummyManager->IsMaster(sessionID)) {
		return;
	}

	GameObject* pTargetObj = g_objectManager.GetObjectPtrOrNull(deleteTank->objectId);
	Dummy* pDummy = g_pDummyManager->GetDummyByUserID(pTargetObj->GetOwnerID());
	pTargetObj = nullptr;

	if (pDummy != nullptr) {
		// 더미에서 Tank 제거	
		pDummy->ClearTank();
	}

	g_objectManager.RemoveObject(deleteTank->objectId);
}

void DummyPacketHandler::OnStartMove(const PACKET_SC_START_MOVE* startMove, UINT32 sessionID)
{
	if (!g_pDummyManager->IsMaster(sessionID)) {
		return;
	}
	// 이동 시작은 로컬에서 반드시 성공하며, 선처리이므로 기본적으로 무시. 
}

void DummyPacketHandler::OnEndMove(const PACKET_SC_END_MOVE* endMove, UINT32 sessionID)
{
	if (!g_pDummyManager->IsMaster(sessionID)) {
		return;
	}
	// 이동 종료도 로컬에서 반드시 성공. 
}

void DummyPacketHandler::OnMoving(const PACKET_SC_MOVING* moving, UINT32 sessionID)
{
	if (!g_pDummyManager->IsMaster(sessionID)) {
		return;
	}

	g_objectManager.SetObjectInputStateByServer(moving->objectId, moving->inputState);
	g_objectManager.UpdateObjectTransformFromServer(moving->objectId, &moving->transform);

}

void DummyPacketHandler::OnSnapshot(const PACKET_SC_SNAPSHOT* snapshot, UINT32 sessionID)
{
	if (!g_pDummyManager->IsMaster(sessionID)) {
		return;
	}
	if (snapshot->countObjects == 0) {
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

void DummyPacketHandler::OnFireMachineGun(const PACKET_SC_FIRE_MACHINE_GUN* fireMachineGun, UINT32 sessionID)
{
	if (!g_pDummyManager->IsMaster(sessionID)) {
		return;
	}
	g_objectManager.CreateProjectile(fireMachineGun->objectId, &fireMachineGun->transform, fireMachineGun->ownerId);
}

void DummyPacketHandler::OnObjectHit(const PACKET_SC_OBJECT_HIT* objectHit, UINT32 sessionID)
{

}

void DummyPacketHandler::OnRespawnTank(const PACKET_SC_RESPAWN_TANK* respawnTank, UINT32 sessionID)
{
	if (!g_pDummyManager->IsMaster(sessionID)) {
		return;
	}
	GameObject* pTank = g_objectManager.GetObjectPtrOrNull(respawnTank->tankId);
	pTank->Respawn();
}
