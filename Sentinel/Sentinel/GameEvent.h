#pragma once

#include "stdafx.h"

#include "GameStruct.h"

const int ID_LENGTH_MAX = 16;
const int PW_LENGTH_MAX = 16;

// [Kind3][Kind2][Kind1][SC, CS] 
enum EGameEventCode
{
	GAME_EVENT_CODE_CS_LOGIN,
	GAME_EVENT_CODE_SC_LOGIN,
	GAME_EVENT_CODE_CS_LOAD_PLAYER_STAT,
	GAME_EVENT_CODE_SC_LOAD_PLAYER_STAT,
	GAME_EVENT_CODE_SC_PLAYER_ID = 0x00010102,
	GAME_EVENT_CODE_SC_SNAPSHOT = 0x00010202,
	GAME_EVENT_CODE_SC_CREATE_TANK = 0x01010102,
	GAME_EVENT_CODE_SC_DELETE_TANK = 0x01010202,
	GAME_EVENT_CODE_SC_RESPAWN_TANK = 0x01010302,
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
	GAME_EVENT_CODE_SC_DELETE_OBSTACLE = 0x02010202,
};

struct GameNetEvent
{
	EGameEventCode evCode;
};


struct PACKET_CS_LOGIN
{
	WCHAR id[ID_LENGTH_MAX + 1];
	WCHAR pw[PW_LENGTH_MAX + 1];
};

struct PACKET_SC_LOGIN
{
	BOOL result;
	UINT32 playerKey;
	int hitCount;
	int killCount;
	int deathCount;
};

struct PACKET_CS_LOAD_PLAYER_STAT
{
	UINT32 playerKey;
};

struct PACKET_SC_LOAD_PLAYER_STAT
{
	int hitCount;
	int killCount;
	int deathCount;
};


struct PACKET_SC_PLAYER_ID
{
	UINT32 id;
};

struct PACKET_SC_CREATE_TANK
{
	UINT32 ownerId;
	UINT16 objectId;
	Transform transform;
};

struct PACKET_SC_DELETE_TANK
{
	UINT16 objectId;
};


const char FLAG_MOVE_FORWARD =	0b00000001;
const char FLAG_MOVE_BACKWARD =	0b00000010;
const char FLAG_ROTATE_LEFT =	0b00000100;
const char FLAG_ROTATE_RIGHT =	0b00001000;

struct PACKET_CS_START_MOVE
{
	char movementFlag; // [][][][][Right][Left][backward][forward]
};

struct PACKET_SC_START_MOVE
{
	char movementFlag;
	UINT16 objectId;
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
	UINT16 objectId;
	Transform transform;
};

struct PACKET_CS_MOVING
{
	Transform transform;
};

struct PACKET_SC_MOVING
{
	UINT16 objectId;
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
	UINT32 shooter;
	UINT32 target;
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

struct PACKET_SC_RESPAWN_TANK
{
	UINT16 tankId;
	Vector3 position;
};

class GamePacket
{
public:
	static BOOL Validate(BYTE* pGameEvent, UINT32 senderId);
	static void HandlePacket(BYTE* pGameEvent, UINT32 senderId);

	static void SendSnapshot(UINT32 sessionId);
	static void SendTankHit(UINT32 sessionId, UINT16 tankId, UINT16 projectileId);
	static void BroadcastDeleteTank(UINT16 tankId);
	static void BroadcastTankHit(UINT16 tankId, UINT16 projectileId, UINT32 shooterId, UINT32 targetId);
	static void BroadcastCreateObstacle(UINT16 obstacleId, Transform* pTransform);
	static void BroadcastDeleteObstacle(UINT16 obstacleId, UINT32 shooterId);
	static void BroadcastRespawnTank(UINT16 tankId);
private:
	static void HandleLogin(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidateLogin(BYTE* pGameEvent, UINT32 senderId);

	static void HandleStartMove(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidateStartMove(BYTE* pGameEvent, UINT32 senderId);

	static void HandleEndMove(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidateEndMove(BYTE* pGameEvent, UINT32 senderId);

	static void HandleMoving(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidateMoving(BYTE* pGameEvent, UINT32 senderId);

	static void HandleShoot(BYTE* pGameEvent, UINT32 senderId);
	static BOOL ValidateShoot(BYTE* pGameEvent, UINT32 senderId);

	
};


