#pragma once

#include "GameStruct.h"

// [Kind3][Kind2][Kind1][SC, CS] 
enum EGameEventCode
{
	GAME_EVENT_CODE_SC_PLAYER_ID = 0x00010102,
	GAME_EVENT_CODE_SC_SNAPSHOT = 0x00010202,
	GAME_EVENT_CODE_CS_CREATE_TANK = 0x01010101,
	GAME_EVENT_CODE_SC_CREATE_TANK = 0x01010102,
	GAME_EVENT_CODE_CS_DELETE_TANK = 0x01010201,
	GAME_EVENT_CODE_SC_DELETE_TANK = 0x01010202,
	GAME_EVENT_CODE_CS_START_MOVE = 0x02010101,
	GAME_EVENT_CODE_SC_START_MOVE = 0x02010102,
	GAME_EVENT_CODE_CS_END_MOVE = 0x02010201,
	GAME_EVENT_CODE_SC_END_MOVE = 0x02010202,
	GAME_EVENT_CODE_CS_MOVING = 0x02010301,
	GAME_EVENT_CODE_SC_MOVING = 0x02010302,
	GAME_EVENT_CODE_CS_SHOOT = 0x02010401,
	GAME_EVENT_CODE_SC_SHOOT = 0x02010402,
	GAME_EVENT_CODE_SC_TANK_HIT = 0x02010502,
};

struct GameNetEvent
{
	EGameEventCode evCode;
};

struct PACKET_SC_PLAYER_ID
{
	UINT32 id;
};

struct PACKET_CS_CREATE_TANK
{
	Transform transform;
};

struct PACKET_SC_CREATE_TANK
{
	UINT32 ownerId;
	UINT32 objectId;
	Transform transform;
};

struct PACKET_CS_DELETE_TANK
{
	UINT32 objectId;
};

struct PACKET_SC_DELETE_TANK
{
	UINT32 objectId;
};


const char FLAG_MOVE_FORWARD	= 0b00000001;
const char FLAG_MOVE_BACKWARD	= 0b00000010;
const char FLAG_ROTATE_LEFT		= 0b00000100;
const char FLAG_ROTATE_RIGHT	= 0b00001000;

struct PACKET_CS_START_MOVE
{
	char movementFlag; // [][][][][Right][Left][backward][forward]
};

struct PACKET_SC_START_MOVE
{
	char movementFlag;
	UINT32 objectId;
	Transform transform; // 클라이언트가 확인하기 위한 용도
};

struct PACKET_CS_END_MOVE
{
	char movementFlag;
	Transform transform; // 보정용
};

struct PACKET_SC_END_MOVE
{
	char movementFlag;
	UINT32 objectId;
	Transform transform;
};

struct PACKET_CS_MOVING
{
	Transform transform;
};

struct PACKET_SC_MOVING
{
	UINT32 objectId;
	Transform transform;
};

struct PACKET_OBJECT_INFO
{
	EGameObjectKind kind;
	UINT16 objectId;
	UINT32 ownerId;
	Transform transform;
};

struct PACKET_SC_SNAPSHOT
{
	UINT16 countObjects;
};

struct PACKET_CS_SHOOT
{
	Transform transform;
};

struct PACKET_SC_SHOOT
{
	UINT16 objectId;
	UINT32 ownerId;
	Transform transform;
};

struct PACKET_SC_TANK_HIT
{
	UINT16 tankId;
	UINT16 projectileId;
};

class GamePacket
{
public:
	static BOOL Validate(BYTE* pGameEvent, UINT32 senderId);
	static void HandlePacket(BYTE* pGameEvent, UINT32 senderId);


	static void SendCreateTank(Transform* pTransform);
	static void SendDeleteTank(UINT32 objectId);

	static void SendStartMove(const Transform* pTankTransform, char moveFlag);
	static void SendEndMove(const Transform* pTankTransform, char moveFlag);
	static void SendMoving(const Transform* pTankTransform);
	static void SendShoot(const Transform* pTurretTransform);

private:
	static void HandlePlayerId(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidatePlayerId(BYTE* pGameEvent, UINT32 senderId);

	static void HandleCreateTank(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidateCreateTank(BYTE* pGameEvent, UINT32 senderId);

	static void HandleDeleteTank(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidateDeleteTank(BYTE* pGameEvent, UINT32 senderId);

	static void HandleStartMove(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidateStartMove(BYTE* pGameEvent, UINT32 senderId);

	static void HandleEndMove(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidateEndMove(BYTE* pGameEvent, UINT32 senderId);

	static void HandleMoving(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidateMoving(BYTE* pGameEvent, UINT32 senderId);

	static void HandleSnapshot(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidateSnapshot(BYTE* pGameEvent, UINT32 senderId);

	static void HandleShoot(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidateShoot(BYTE* pGameEvent, UINT32 senderId);
	
	static void HandleTankHit(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidateTankHit(BYTE* pGameEvent, UINT32 senderId);


};
