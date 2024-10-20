#pragma once

#include "stdafx.h"

#include "GameStruct.h"

// [Kind3][Kind2][Kind1][SC, CS] 
enum EGameEventCode
{
	GAME_EVENT_CODE_SC_PLAYER_ID = 0x00010102,
	GAME_EVENT_CODE_SC_SNAPSHOT = 0x00010202,
	GAME_EVENT_CODE_CS_CREATE_TANK = 0x01010101, // Deprecated
	GAME_EVENT_CODE_SC_CREATE_TANK = 0x01010102,
	GAME_EVENT_CODE_CS_DELETE_TANK = 0x01010201, // Deprecated
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
	GAME_EVENT_CODE_SC_CREATE_OBSTACLE = 0x02010102,
	GAME_EVENT_CODE_SC_DELETE_OBSTACLE = 0x02010202
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

struct PACKET_CS_START_MOVE
{
	Transform transform;
};

struct PACKET_SC_START_MOVE
{
	UINT32 objectId;
	Transform transform;
};

struct PACKET_CS_END_MOVE
{
	Transform transform;
};

struct PACKET_SC_END_MOVE
{
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

struct PACKET_SC_CREATE_OBSTACLE
{
	UINT16 obstacleId;
	Transform transform;
};

struct PACKET_SC_DELETE_OBSTACLE
{
	UINT16 obstacleId;
	UINT32 shooterId;
};

class GamePacket
{
public:
	static BOOL Validate(BYTE* pGameEvent, UINT32 senderId);
	static void HandlePacket(BYTE* pGameEvent, UINT32 senderId);

	static void SendSnapshot(UINT32 sessionId);
	static void SendTankHit(UINT32 sessionId, UINT16 tankId, UINT16 projectileId);
	static void BroadcastTankHit(UINT16 tankId, UINT16 projectileId);
	static void BroadcastCreateObstacle(UINT16 obstacleId, Transform* pTransform);
	static void BroadcastDeleteObstacle(UINT16 obstacleId, UINT32 shooterId);
private:
	
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

	static void HandleShoot(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidateShoot(BYTE* pGameEvent, UINT32 senderId);
};


