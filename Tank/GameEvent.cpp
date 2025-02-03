#include "GameEvent.h"
#include "Global.h"
#include "NetCore.h"
#include "Tank.h"
#include "AllocObjectManager.h"
#include "Game.h"
#include "Player.h"
#include "CommonData.h"

BOOL GamePacket::Validate(BYTE* pGameEvent, UINT32 senderId)
{
	return 0;
}

void GamePacket::HandlePacket(BYTE* pGameEvent, UINT32 senderId)
{
	EGameEventCode* evCode = (EGameEventCode*)pGameEvent;
	switch (*evCode) {
	case GAME_EVENT_CODE_SC_LOGIN:
		HandleLoginResult(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_SC_SNAPSHOT:
		HandleSnapshot(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_SC_CREATE_TANK:
		HandleCreateTank(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_SC_DELETE_TANK:
		HandleDeleteTank(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_SC_START_MOVE:
		HandleStartMove(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_SC_END_MOVE:
		HandleEndMove(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_SC_MOVING:
		HandleMoving(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_SC_FIRE_MACHINE_GUN:
		HandleFireMachineGun(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_SC_TANK_HIT:
		HandleTankHit(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_SC_RESPAWN_TANK:
		HandleRespawnTank(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_SC_OBJECT_HIT:
		HandleObjectHit(pGameEvent, senderId);
		break;
	default:
		__debugbreak();
		break;
	}
}


void GamePacket::HandleLoginResult(BYTE* pGameEvent, UINT32 senderId)
{
	PACKET_SC_LOGIN* pScLogin = (PACKET_SC_LOGIN*)(pGameEvent + sizeof(EGameEventCode));
	if (pScLogin->result == FALSE) {
		// End game.
		Game::Shutdown();
		return;
	}

	g_pPlayer->OnSuccessLogin(pScLogin->userDBIndex, { pScLogin->hitCount , pScLogin->killCount , pScLogin->deathCount });
}

void GamePacket::HandleCreateTank(BYTE* pGameEvent, UINT32 senderId)
{
	PACKET_SC_CREATE_TANK* pScCreateTank = (PACKET_SC_CREATE_TANK*)(pGameEvent + sizeof(EGameEventCode));
	
	Tank* pTank = g_objectManager.CreateTank(pScCreateTank->objectId, pScCreateTank->ownerId);
	if (pScCreateTank->ownerId == g_pPlayer->GetUserID()) {
		g_pPlayer->SetTank(pTank);
		g_isTankCreateRequest = false;
	}
}

void GamePacket::HandleDeleteTank(BYTE* pGameEvent, UINT32 senderId)
{
	PACKET_SC_DELETE_TANK* pScDeleteTank = (PACKET_SC_DELETE_TANK*)(pGameEvent + sizeof(EGameEventCode));

	if (pScDeleteTank->objectId.equals(g_pPlayer->GetTankID())) {
		g_pPlayer->ClearTank();
	}
	g_objectManager.RemoveObject(pScDeleteTank->objectId);
}

void GamePacket::HandleStartMove(BYTE* pGameEvent, UINT32 senderId)
{
	PACKET_SC_START_MOVE* pScStartMove = (PACKET_SC_START_MOVE*)(pGameEvent + sizeof(EGameEventCode));

	if (pScStartMove->objectId.equals(g_pPlayer->GetTankID())) {
		// hmm..
	}
	else {
		g_objectManager.SetObjectInputStateByServer(pScStartMove->objectId, pScStartMove->inputState);
		g_objectManager.UpdateObjectTransformFromServer(pScStartMove->objectId, &pScStartMove->transform);
	}
}

void GamePacket::HandleEndMove(BYTE* pGameEvent, UINT32 senderId)
{	
	PACKET_SC_END_MOVE* pScEndMove = (PACKET_SC_END_MOVE*)(pGameEvent + sizeof(EGameEventCode));
	
	if (pScEndMove->objectId.equals(g_pPlayer->GetTankID())) {
		// overrided..
	}

	g_objectManager.SetObjectInputStateByServer(pScEndMove->objectId, PLAYER_INPUT_NONE);
	g_objectManager.UpdateObjectTransformFromServer(pScEndMove->objectId, &pScEndMove->transform);
}

void GamePacket::HandleMoving(BYTE* pGameEvent, UINT32 senderId)
{
	PACKET_SC_MOVING* pScMoving = (PACKET_SC_MOVING*)(pGameEvent + sizeof(EGameEventCode));
	
	if (pScMoving->objectId.equals(g_pPlayer->GetTankID())) {
		// update if transforms are diffrent
	}

	g_objectManager.SetObjectInputStateByServer(pScMoving->objectId, pScMoving->inputState);
	g_objectManager.UpdateObjectTransformFromServer(pScMoving->objectId, &pScMoving->transform);
}

void GamePacket::HandleSnapshot(BYTE* pGameEvent, UINT32 senderId)
{
	PACKET_SC_SNAPSHOT* pScSnapshot = (PACKET_SC_SNAPSHOT*)(pGameEvent + sizeof(EGameEventCode));
	if (pScSnapshot->countObjects == 0) {
		// Todo: set start flag
		return;
	}

	PACKET_OBJECT_INFO* pObjInfo = (PACKET_OBJECT_INFO*)(pScSnapshot + 1);
	for (UINT16 i = 0; i < pScSnapshot->countObjects; ++i) {
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

void GamePacket::HandleFireMachineGun(BYTE* pGameEvent, UINT32 senderId)
{
	PACKET_SC_SHOOT* pScShoot = (PACKET_SC_SHOOT*)(pGameEvent + sizeof(EGameEventCode));
	g_objectManager.CreateProjectile(pScShoot->objectId, &pScShoot->transform, pScShoot->ownerId);
}

void GamePacket::HandleObjectHit(BYTE* pGameEvent, UINT32 senderID)
{
	PACKET_SC_OBJECT_HIT* pScObjectHit = (PACKET_SC_OBJECT_HIT*)(pGameEvent + sizeof(EGameEventCode));
	ObjectID oneID = pScObjectHit->oneID;
	ObjectID anotherID = pScObjectHit->anotherID;

	GameObject* pOne = g_objectManager.GetObjectPtrOrNull(oneID);
	GameObject* pAnother = g_objectManager.GetObjectPtrOrNull(anotherID);

	pOne->OnHitServer(g_currentGameTick, pAnother);
	pAnother->OnHitServer(g_currentGameTick, pOne);
}

void GamePacket::HandleTankHit(BYTE* pGameEvent, UINT32 senderId)
{
	__debugbreak();
	//PACKET_SC_TANK_HIT* pScTankHit = (PACKET_SC_TANK_HIT*)(pGameEvent + sizeof(EGameEventCode));
	//Tank* pTank = (Tank*)g_objectManager.GetObjectPtrOrNull(pScTankHit->tankId);
	//
	//if (g_pPlayer->GetUserID() == pScTankHit->target) {
	//	g_pPlayer->IncreaseHit();
	//}

	//if (g_pPlayer->GetUserID() == pScTankHit->shooter) {
	//	g_pPlayer->IncreaseKill();
	//}

	//// pTank->OnHitByProjectile(g_currentGameTick);

	//Projectile* pProjectile = (Projectile*)g_objectManager.GetObjectPtrOrNull(pScTankHit->projectileId);
	//pProjectile->OnHitTank(g_currentGameTick);
}

void GamePacket::HandleCreateObstacle(BYTE* pGameEvent, UINT32 senderId)
{

}

void GamePacket::HandleDeleteObstacle(BYTE* pGameEvent, UINT32 senderId)
{

}

void GamePacket::HandleRespawnTank(BYTE* pGameEvent, UINT32 senderId)
{

	PACKET_SC_RESPAWN_TANK* pScTankHit = (PACKET_SC_RESPAWN_TANK*)(pGameEvent + sizeof(EGameEventCode));
	GameObject* pTank = g_objectManager.GetObjectPtrOrNull(pScTankHit->tankId);
	pTank->Respawn();
}

void GamePacket::SendLogin(const std::wstring& wID, const std::wstring& wPw)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_LOGIN) + sizeof(EGameEventCode);

	BYTE pRawPacket[contentsMsgSize] = { 0, };

	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_LOGIN* pLogin = (PACKET_CS_LOGIN*)(pRawPacket + sizeof(EGameEventCode));

	const WCHAR* rawID = wID.c_str();
	const WCHAR* rawPW = wPw.c_str();
	memcpy(&pLogin->id, rawID, sizeof(WCHAR) * (wID.length() + 1));
	memcpy(&pLogin->pw, rawPW, sizeof(WCHAR) * (wPw.length() + 1));

	g_pNetCore->SendMessageTo(g_pPlayer->GetSessionID(), pRawPacket, contentsMsgSize);
}

void GamePacket::SendStartMove(const Transform* pTankTransform, PlayerInputState inputState)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_START_MOVE) + sizeof(EGameEventCode);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_START_MOVE* pContentsMsgBody = (PACKET_CS_START_MOVE*)(pRawPacket + sizeof(EGameEventCode));

	*pEvCode = GAME_EVENT_CODE_CS_START_MOVE;
	pContentsMsgBody->inputState = inputState;
	g_pNetCore->SendMessageTo(g_pPlayer->GetSessionID(), pRawPacket, contentsMsgSize);
}

void GamePacket::SendEndMove(const Transform* pTankTransform)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_END_MOVE) + sizeof(EGameEventCode);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_END_MOVE* pContentsMsgBody = (PACKET_CS_END_MOVE*)(pRawPacket + sizeof(EGameEventCode));

	*pEvCode = GAME_EVENT_CODE_CS_END_MOVE;
	memcpy(&pContentsMsgBody->transform, pTankTransform, sizeof(Transform));
	g_pNetCore->SendMessageTo(g_pPlayer->GetSessionID(), pRawPacket, contentsMsgSize);
}

void GamePacket::SendMoving(const Transform* pTankTransform, PlayerInputState moveFlag)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_MOVING) + sizeof(EGameEventCode);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_MOVING* pContentsMsgBody = (PACKET_CS_MOVING*)(pRawPacket + sizeof(EGameEventCode));

	*pEvCode = GAME_EVENT_CODE_CS_MOVING;
	memcpy(&pContentsMsgBody->transform, pTankTransform, sizeof(Transform));
	pContentsMsgBody->inputState = moveFlag;
	g_pNetCore->SendMessageTo(g_pPlayer->GetSessionID(), pRawPacket, contentsMsgSize);
}

void GamePacket::SendFireMachineGun(const Transform* pTurretTransform)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_SHOOT) + sizeof(EGameEventCode);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_SHOOT* pContentsMsgBody = (PACKET_CS_SHOOT*)(pRawPacket + sizeof(EGameEventCode));

	*pEvCode = GAME_EVENT_CODE_CS_FIRE_MACHINE_GUN;
	memcpy(&pContentsMsgBody->transform, pTurretTransform, sizeof(Transform));
	g_pNetCore->SendMessageTo(g_pPlayer->GetSessionID(), pRawPacket, contentsMsgSize);
}
