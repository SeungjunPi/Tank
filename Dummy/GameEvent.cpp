#include "GameEvent.h"
#include "Global.h"
#include "NetCore.h"
#include "Tank.h"
#include "AllocObjectManager.h"
#include "Game.h"
#include "Player.h"
#include "DummyManager.h"
#include "Dummy.h"

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
	case GAME_EVENT_CODE_SC_SHOOT:
		HandleShoot(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_SC_TANK_HIT:
		HandleTankHit(pGameEvent, senderId);
		break;
	case GAME_EVENT_CODE_SC_RESPAWN_TANK:
		HandleRespawnTank(pGameEvent, senderId);
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

	Dummy* pDummy = g_pDummyManager->GetDummyBySessionID(senderId);
	// Find dummy and call
	pDummy->OnSuccessLogin(pScLogin->userDBIndex, { pScLogin->hitCount , pScLogin->killCount , pScLogin->deathCount });
}

void GamePacket::HandleCreateTank(BYTE* pGameEvent, UINT32 senderId)
{
	PACKET_SC_CREATE_TANK* pScCreateTank = (PACKET_SC_CREATE_TANK*)(pGameEvent + sizeof(EGameEventCode));

	Dummy* pDummy = g_pDummyManager->GetDummyBySessionID(senderId);
	// TODO: 중복 생성의 가능성이 있으므로, 적절하게 처리.
	Tank* pTank = g_objectManager.CreateTank(pScCreateTank->objectId, pScCreateTank->ownerId);
	if (pScCreateTank->ownerId == pDummy->GetUserID()) {
		pDummy->SetTank(pTank);
	}
}

void GamePacket::HandleDeleteTank(BYTE* pGameEvent, UINT32 senderId)
{
	PACKET_SC_DELETE_TANK* pScDeleteTank = (PACKET_SC_DELETE_TANK*)(pGameEvent + sizeof(EGameEventCode));

	Dummy* pDummy = g_pDummyManager->GetDummyBySessionID(senderId);
	if (pScDeleteTank->objectId.equals(pDummy->GetTankID())) {
		pDummy->ClearTank();
	}
	g_objectManager.RemoveObject(pScDeleteTank->objectId);
}

void GamePacket::HandleStartMove(BYTE* pGameEvent, UINT32 senderId)
{
	PACKET_SC_START_MOVE* pScStartMove = (PACKET_SC_START_MOVE*)(pGameEvent + sizeof(EGameEventCode));

	Dummy* pDummy = g_pDummyManager->GetDummyBySessionID(senderId);
	if (pScStartMove->objectId.equals(pDummy->GetTankID())) {
		g_lastOwnTankSyncTick = g_currentGameTick;
	}
	else {
		if (pScStartMove->movementFlag & FLAG_MOVE_FORWARD) {
			g_objectManager.StartTankMove(pScStartMove->objectId, EMovement::FORWARD);
		}
		if (pScStartMove->movementFlag & FLAG_MOVE_BACKWARD) {
			g_objectManager.StartTankMove(pScStartMove->objectId, EMovement::BACKWARD);
		}
		if (pScStartMove->movementFlag & FLAG_ROTATE_LEFT) {
			g_objectManager.StartTankRotate(pScStartMove->objectId, ERotation::LEFT);
		}
		if (pScStartMove->movementFlag & FLAG_ROTATE_RIGHT) {
			g_objectManager.StartTankRotate(pScStartMove->objectId, ERotation::RIGHT);
		}
	}
}

void GamePacket::HandleEndMove(BYTE* pGameEvent, UINT32 senderId)
{
	PACKET_SC_END_MOVE* pScEndMove = (PACKET_SC_END_MOVE*)(pGameEvent + sizeof(EGameEventCode));
	
	Dummy* pDummy = g_pDummyManager->GetDummyBySessionID(senderId);
	if (pScEndMove->objectId.equals(pDummy->GetTankID())) {
		// TODO: Log "Adjusted based on ther server's transform\n"
		g_lastOwnTankSyncTick = g_currentGameTick;
	}

	if (pScEndMove->movementFlag & FLAG_MOVE_FORWARD) {
		g_objectManager.EndTankMove(pScEndMove->objectId, EMovement::FORWARD, &pScEndMove->transform);
	}
	if (pScEndMove->movementFlag & FLAG_MOVE_BACKWARD) {
		g_objectManager.EndTankMove(pScEndMove->objectId, EMovement::BACKWARD, &pScEndMove->transform);
	}
	if (pScEndMove->movementFlag & FLAG_ROTATE_LEFT) {
		g_objectManager.EndTankRotate(pScEndMove->objectId, ERotation::LEFT, &pScEndMove->transform);
	}
	if (pScEndMove->movementFlag & FLAG_ROTATE_RIGHT) {
		g_objectManager.EndTankRotate(pScEndMove->objectId, ERotation::RIGHT, &pScEndMove->transform);
	}
}

void GamePacket::HandleMoving(BYTE* pGameEvent, UINT32 senderId)
{
	PACKET_SC_MOVING* pScMoving = (PACKET_SC_MOVING*)(pGameEvent + sizeof(EGameEventCode));
	
	Dummy* pDummy = g_pDummyManager->GetDummyBySessionID(senderId);
	if (pScMoving->objectId.equals(pDummy->GetTankID())) {
		// update if transforms are diffrent
	}
	else {
		g_objectManager.UpdateObjectTransform(pScMoving->objectId, &pScMoving->transform);
	}
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
			pTank->UpdateTransform(&pObjInfo->transform);
			break;
		}
		case GAME_OBJECT_TYPE_PROJECTILE:
		{
			Projectile* pProjectile = g_objectManager.CreateProjectile(pObjInfo->objectId, &pObjInfo->transform, pObjInfo->ownerId);
			pProjectile->UpdateTransform(&pObjInfo->transform);
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

void GamePacket::HandleShoot(BYTE* pGameEvent, UINT32 senderId)
{
	PACKET_SC_SHOOT* pScShoot= (PACKET_SC_SHOOT*)(pGameEvent + sizeof(EGameEventCode));
	pScShoot->objectId;
	pScShoot->ownerId;
	pScShoot->transform;

	g_objectManager.CreateProjectile(pScShoot->objectId, &pScShoot->transform, pScShoot->ownerId);
}

void GamePacket::HandleTankHit(BYTE* pGameEvent, UINT32 senderId)
{
	PACKET_SC_TANK_HIT* pScTankHit = (PACKET_SC_TANK_HIT*)(pGameEvent + sizeof(EGameEventCode));
	Tank* pTank = (Tank*)g_objectManager.GetObjectPtrOrNull(pScTankHit->tankId);
	Dummy* pDummy = g_pDummyManager->GetDummyBySessionID(senderId);
	if (pDummy->GetUserID() == pScTankHit->target) {
		pDummy->IncreaseHit();
	}

	if (pDummy->GetUserID() == pScTankHit->shooter) {
		pDummy->IncreaseKill();
	}

	pTank->OnHitByProjectile(g_currentGameTick);

	Projectile* pProjectile = (Projectile*)g_objectManager.GetObjectPtrOrNull(pScTankHit->projectileId);
	pProjectile->OnHitTank(g_currentGameTick);
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

void GamePacket::SendLogin(const std::wstring& wID, const std::wstring& wPw, UINT32 sessionID)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_LOGIN) + sizeof(EGameEventCode);

	BYTE pRawPacket[contentsMsgSize] = { 0, };

	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_LOGIN* pLogin = (PACKET_CS_LOGIN*)(pRawPacket + sizeof(EGameEventCode));

	const WCHAR* rawID = wID.c_str();
	const WCHAR* rawPW = wPw.c_str();
	memcpy(&pLogin->id, rawID, sizeof(WCHAR) * (wID.length() + 1));
	memcpy(&pLogin->pw, rawPW, sizeof(WCHAR) * (wPw.length() + 1));

	g_pNetCore->SendMessageTo(sessionID, pRawPacket, contentsMsgSize);
}

void GamePacket::SendStartMove(const Transform* pTankTransform, char moveFlag, UINT32 sessionID)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_START_MOVE) + sizeof(EGameEventCode);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_START_MOVE* pContentsMsgBody = (PACKET_CS_START_MOVE*)(pRawPacket + sizeof(EGameEventCode));

	*pEvCode = GAME_EVENT_CODE_CS_START_MOVE;
	pContentsMsgBody->movementFlag = moveFlag;
	g_pNetCore->SendMessageTo(sessionID, pRawPacket, contentsMsgSize);
}

void GamePacket::SendEndMove(const Transform* pTankTransform, char moveFlag, UINT32 sessionID)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_END_MOVE) + sizeof(EGameEventCode);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_END_MOVE* pContentsMsgBody = (PACKET_CS_END_MOVE*)(pRawPacket + sizeof(EGameEventCode));

	*pEvCode = GAME_EVENT_CODE_CS_END_MOVE;
	memcpy(&pContentsMsgBody->transform, pTankTransform, sizeof(Transform));
	pContentsMsgBody->movementFlag = moveFlag;
	g_pNetCore->SendMessageTo(sessionID, pRawPacket, contentsMsgSize);
}

void GamePacket::SendMoving(const Transform* pTankTransform, UINT32 sessionID)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_MOVING) + sizeof(EGameEventCode);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_MOVING* pContentsMsgBody = (PACKET_CS_MOVING*)(pRawPacket + sizeof(EGameEventCode));

	*pEvCode = GAME_EVENT_CODE_CS_MOVING;
	memcpy(&pContentsMsgBody->transform, pTankTransform, sizeof(Transform));
	g_pNetCore->SendMessageTo(sessionID, pRawPacket, contentsMsgSize);
}

void GamePacket::SendFireMachineGun(const Transform* pTurretTransform, UINT32 sessionID)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_SHOOT) + sizeof(EGameEventCode);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_SHOOT* pContentsMsgBody = (PACKET_CS_SHOOT*)(pRawPacket + sizeof(EGameEventCode));

	*pEvCode = GAME_EVENT_CODE_CS_SHOOT;
	memcpy(&pContentsMsgBody->transform, pTurretTransform, sizeof(Transform));
	g_pNetCore->SendMessageTo(sessionID, pRawPacket, contentsMsgSize);
}
