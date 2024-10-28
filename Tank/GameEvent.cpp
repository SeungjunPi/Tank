#include "GameEvent.h"
#include "Global.h"
#include "NetCore.h"
#include "Tank.h"
#include "AllocObjectManager.h"

BOOL GamePacket::Validate(BYTE* pGameEvent, UINT32 senderId)
{
	return 0;
}

void GamePacket::HandlePacket(BYTE* pGameEvent, UINT32 senderId)
{
	EGameEventCode* evCode = (EGameEventCode*)pGameEvent;
	switch (*evCode) {
	case GAME_EVENT_CODE_SC_PLAYER_ID:
		HandlePlayerId(pGameEvent, senderId);
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
	default:
		__debugbreak();
		break;
	}
}


void GamePacket::HandlePlayerId(BYTE* pGameEvent, UINT32 senderId)
{
	bool isValid = ValidatePlayerId(pGameEvent, senderId);
	if (!isValid) {
		return;
	}

	PACKET_SC_PLAYER_ID* pScPlayerId = (PACKET_SC_PLAYER_ID*)(pGameEvent + sizeof(EGameEventCode));
	g_playerId = pScPlayerId->id;
}

BOOL GamePacket::ValidatePlayerId(BYTE* pGameEvent, UINT32 senderId)
{
	return true;
}

void GamePacket::HandleCreateTank(BYTE* pGameEvent, UINT32 senderId)
{
	bool isValid = ValidateCreateTank(pGameEvent, senderId);
	if (!isValid) {
		return;
	}

	PACKET_SC_CREATE_TANK* pScCreateTank = (PACKET_SC_CREATE_TANK*)(pGameEvent + sizeof(EGameEventCode));
	
	Tank* pTank = g_objectManager.CreateTank(pScCreateTank->objectId);
	if (pScCreateTank->ownerId == g_playerId) {
		g_pPlayerTank = pTank;
		g_isTankCreateRequest = false;
	}
}

BOOL GamePacket::ValidateCreateTank(BYTE* pGameEvent, UINT32 senderId)
{
	return true;
}

void GamePacket::HandleDeleteTank(BYTE* pGameEvent, UINT32 senderId)
{
	bool isValid = ValidateDeleteTank(pGameEvent, senderId);
	if (!isValid) {
		return;
	}

	PACKET_SC_DELETE_TANK* pScDeleteTank = (PACKET_SC_DELETE_TANK*)(pGameEvent + sizeof(EGameEventCode));
	g_objectManager.RemoveObject(GAME_OBJECT_KIND_TANK, pScDeleteTank->objectId);
	if (pScDeleteTank->objectId == g_pPlayerTank->GetID()) {
		g_pPlayerTank = NULL;
	}
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

	PACKET_SC_START_MOVE* pScStartMove = (PACKET_SC_START_MOVE*)(pGameEvent + sizeof(EGameEventCode));
	if (g_pPlayerTank != nullptr && pScStartMove->objectId == g_pPlayerTank->GetID()) {
		// update if transforms are diffrent
	}
	else {
		if (pScStartMove->movementFlag & FLAG_MOVE_FORWARD) {
			g_objectManager.StartTankMove(pScStartMove->objectId, EMOVEMENT::FORWARD);
		}
		if (pScStartMove->movementFlag & FLAG_MOVE_BACKWARD) {
			g_objectManager.StartTankMove(pScStartMove->objectId, EMOVEMENT::BACKWARD);
		}
		if (pScStartMove->movementFlag & FLAG_ROTATE_LEFT) {
			g_objectManager.StartTankRotate(pScStartMove->objectId, EROTATION::LEFT);
		}
		if (pScStartMove->movementFlag & FLAG_ROTATE_RIGHT) {
			g_objectManager.StartTankRotate(pScStartMove->objectId, EROTATION::RIGHT);
		}
	}
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

	PACKET_SC_END_MOVE* pScEndMove = (PACKET_SC_END_MOVE*)(pGameEvent + sizeof(EGameEventCode));
	
	if (g_pPlayerTank != nullptr && pScEndMove->objectId == g_pPlayerTank->GetID()) {
		// TODO: Log "Adjusted based on ther server's transform\n"
	}

	if (pScEndMove->movementFlag & FLAG_MOVE_FORWARD) {
		g_objectManager.EndTankMove(pScEndMove->objectId, EMOVEMENT::FORWARD, &pScEndMove->transform);
	}
	if (pScEndMove->movementFlag & FLAG_MOVE_BACKWARD) {
		g_objectManager.EndTankMove(pScEndMove->objectId, EMOVEMENT::BACKWARD, &pScEndMove->transform);
	}
	if (pScEndMove->movementFlag & FLAG_ROTATE_LEFT) {
		g_objectManager.EndTankRotate(pScEndMove->objectId, EROTATION::LEFT, &pScEndMove->transform);
	}
	if (pScEndMove->movementFlag & FLAG_ROTATE_RIGHT) {
		g_objectManager.EndTankRotate(pScEndMove->objectId, EROTATION::RIGHT, &pScEndMove->transform);
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

	PACKET_SC_MOVING* pScMoving = (PACKET_SC_MOVING*)(pGameEvent + sizeof(EGameEventCode));
	
	if (g_pPlayerTank != nullptr && pScMoving->objectId == g_pPlayerTank->GetID()) {
		// update if transforms are diffrent
	}
	else {
		g_objectManager.UpdateObjectTransform(GAME_OBJECT_KIND_TANK, pScMoving->objectId, &pScMoving->transform);
	}
}

BOOL GamePacket::ValidateMoving(BYTE* pGameEvent, UINT32 senderId)
{
	return true;
}

void GamePacket::HandleSnapshot(BYTE* pGameEvent, UINT32 senderId)
{
	bool isValid = ValidateSnapshot(pGameEvent, senderId);
	if (!isValid) {
		return;
	}

	PACKET_SC_SNAPSHOT* pScSnapshot = (PACKET_SC_SNAPSHOT*)(pGameEvent + sizeof(EGameEventCode));
	if (pScSnapshot->countObjects == 0) {
		// Todo: set start flag
		return;
	}

	PACKET_OBJECT_INFO* pObjInfo = (PACKET_OBJECT_INFO*)(pScSnapshot + 1);
	for (UINT16 i = 0; i < pScSnapshot->countObjects; ++i) {
		switch (pObjInfo->kind) {
		case GAME_OBJECT_KIND_TANK:
		{
			Tank* pTank = g_objectManager.CreateTank(pObjInfo->objectId);
			pTank->UpdateTransform(&pObjInfo->transform);
			break;
		}
		case GAME_OBJECT_KIND_PROJECTILE:
		{
			Projectile* pProjectile = g_objectManager.CreateProjectile(pObjInfo->objectId, &pObjInfo->transform);
			pProjectile->UpdateTransform(&pObjInfo->transform);
			break;
		}
		case GAME_OBJECT_KIND_OBSTACLE:
			//g_objectManager.CreateObstacle(pObjInfo->objectId);
			break;
		default:
			__debugbreak();
		}
		++pObjInfo;
	}

	
}

BOOL GamePacket::ValidateSnapshot(BYTE* pGameEvent, UINT32 senderId)
{
	return true;
}

void GamePacket::HandleShoot(BYTE* pGameEvent, UINT32 senderId)
{
	bool isValid = ValidateShoot(pGameEvent, senderId);
	if (!isValid) {
		return;
	}

	PACKET_SC_SHOOT* pScShoot= (PACKET_SC_SHOOT*)(pGameEvent + sizeof(EGameEventCode));
	pScShoot->objectId;
	pScShoot->ownerId;
	pScShoot->transform;

	g_objectManager.CreateProjectile(pScShoot->objectId, &pScShoot->transform);
}

BOOL GamePacket::ValidateShoot(BYTE* pGameEvent, UINT32 senderId)
{
	return true;
}

void GamePacket::HandleTankHit(BYTE* pGameEvent, UINT32 senderId)
{
	bool isValid = ValidateTankHit(pGameEvent, senderId);
	if (!isValid) {
		return;
	}

	PACKET_SC_TANK_HIT* pScTankHit = (PACKET_SC_TANK_HIT*)(pGameEvent + sizeof(EGameEventCode));
	GameObject* pTank = g_objectManager.GetObjectPtrOrNull(GAME_OBJECT_KIND_TANK, pScTankHit->tankId);
	pTank->OnHit(g_currentGameTick);

	GameObject* pProjectile = g_objectManager.GetObjectPtrOrNull(GAME_OBJECT_KIND_PROJECTILE, pScTankHit->projectileId);
	pProjectile->OnHit(g_currentGameTick);
}

BOOL GamePacket::ValidateTankHit(BYTE* pGameEvent, UINT32 senderId)
{
	return true;
}

void GamePacket::HandleCreateObstacle(BYTE* pGameEvent, UINT32 senderId)
{

}

BOOL GamePacket::ValidateCreateObstacle(BYTE* pGameEvent, UINT32 senderId)
{
	return true;
}

void GamePacket::HandleDeleteObstacle(BYTE* pGameEvent, UINT32 senderId)
{

}

BOOL GamePacket::ValidateDeleteObstacle(BYTE* pGameEvent, UINT32 senderId)
{
	return true;
}

void GamePacket::SendCreateTank(Transform* pTransform)
{	
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_CREATE_TANK) + sizeof(EGameEventCode);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	*pEvCode = EGameEventCode::GAME_EVENT_CODE_CS_CREATE_TANK;
	PACKET_CS_CREATE_TANK* pContentsMessageBody = (PACKET_CS_CREATE_TANK*)(pRawPacket + sizeof(EGameEventCode));
	memcpy(&pContentsMessageBody->transform, pTransform, sizeof(Transform));
	g_pNetCore->SendMessageTo(g_serverId, pRawPacket, contentsMsgSize);
	
	
}

void GamePacket::SendDeleteTank(UINT32 objectId)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_DELETE_TANK) + sizeof(EGameEventCode);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_DELETE_TANK* pContentsMessageBody = (PACKET_CS_DELETE_TANK*)(pRawPacket + sizeof(EGameEventCode));

	*pEvCode = EGameEventCode::GAME_EVENT_CODE_CS_DELETE_TANK;
	pContentsMessageBody->objectId = objectId;
	g_pNetCore->SendMessageTo(g_serverId, pRawPacket, contentsMsgSize);
}

void GamePacket::SendStartMove(const Transform* pTankTransform, char moveFlag)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_START_MOVE) + sizeof(EGameEventCode);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_START_MOVE* pContentsMsgBody = (PACKET_CS_START_MOVE*)(pRawPacket + sizeof(EGameEventCode));

	*pEvCode = GAME_EVENT_CODE_CS_START_MOVE;
	pContentsMsgBody->movementFlag = moveFlag;
	g_pNetCore->SendMessageTo(g_serverId, pRawPacket, contentsMsgSize);
}

void GamePacket::SendEndMove(const Transform* pTankTransform, char moveFlag)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_END_MOVE) + sizeof(EGameEventCode);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_END_MOVE* pContentsMsgBody = (PACKET_CS_END_MOVE*)(pRawPacket + sizeof(EGameEventCode));

	*pEvCode = GAME_EVENT_CODE_CS_END_MOVE;
	memcpy(&pContentsMsgBody->transform, pTankTransform, sizeof(Transform));
	pContentsMsgBody->movementFlag = moveFlag;
	g_pNetCore->SendMessageTo(g_serverId, pRawPacket, contentsMsgSize);
}

void GamePacket::SendMoving(const Transform* pTankTransform)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_MOVING) + sizeof(EGameEventCode);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_MOVING* pContentsMsgBody = (PACKET_CS_MOVING*)(pRawPacket + sizeof(EGameEventCode));

	*pEvCode = GAME_EVENT_CODE_CS_MOVING;
	memcpy(&pContentsMsgBody->transform, pTankTransform, sizeof(Transform));
	g_pNetCore->SendMessageTo(g_serverId, pRawPacket, contentsMsgSize);
}

void GamePacket::SendShoot(const Transform* pTurretTransform)
{
	const UINT32 contentsMsgSize = sizeof(PACKET_CS_SHOOT) + sizeof(EGameEventCode);
	BYTE pRawPacket[contentsMsgSize] = { 0, };
	EGameEventCode* pEvCode = (EGameEventCode*)pRawPacket;
	PACKET_CS_SHOOT* pContentsMsgBody = (PACKET_CS_SHOOT*)(pRawPacket + sizeof(EGameEventCode));

	*pEvCode = GAME_EVENT_CODE_CS_SHOOT;
	memcpy(&pContentsMsgBody->transform, pTurretTransform, sizeof(Transform));
	g_pNetCore->SendMessageTo(g_serverId, pRawPacket, contentsMsgSize);
}
