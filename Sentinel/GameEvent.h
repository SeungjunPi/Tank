#pragma once

#include "SentinelPch.h"
#include "SentinelAliases.h"
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
	UserDBIndex userDBIndex;
	int hitCount;
	int killCount;
	int deathCount;
};

struct PACKET_CS_LOAD_PLAYER_STAT
{
	UserDBIndex userDBIndex;
};

struct PACKET_SC_LOAD_PLAYER_STAT
{
	int hitCount;
	int killCount;
	int deathCount;
};


struct PACKET_SC_PLAYER_ID
{
	UserDBIndex id;
};

struct PACKET_SC_CREATE_TANK
{
	UserDBIndex ownerId;
	ObjectID objectId;
	Transform transform;
};

struct PACKET_SC_DELETE_TANK
{
	ObjectID objectId;
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
	ObjectID objectId;
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
	ObjectID objectId;
	Transform transform;
};

struct PACKET_CS_MOVING
{
	Transform transform;
};

struct PACKET_SC_MOVING
{
	ObjectID objectId;
	Transform transform;
};

struct PACKET_OBJECT_INFO
{
	EGameObjectKind kind;
	ObjectID objectId;
	UserDBIndex ownerId;
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
	ObjectID objectId;
	UserDBIndex ownerId;
	Transform transform;
};

struct PACKET_SC_TANK_HIT
{
	ObjectID tankId;
	ObjectID projectileId;
	UserDBIndex shooter;
	UserDBIndex target;
};

struct PACKET_SC_CREATE_OBSTACLE
{
	ObjectID obstacleId;
	Transform transform;
};

struct PACKET_SC_DELETE_OBSTACLE
{
	ObjectID obstacleId;
	UserDBIndex shooterId;
};

struct PACKET_SC_RESPAWN_TANK
{
	ObjectID tankId;
	Vector3 position;
};

class GamePacket
{
public:
	static BOOL Validate(BYTE* pGameEvent, SessionID senderId);
	static void HandlePacket(BYTE* pGameEvent, SessionID senderId);

	static void SendSnapshot(SessionID sessionId);
	static void SendTankHit(SessionID sessionId, ObjectID tankId, ObjectID projectileId);
	static void BroadcastDeleteTank(ObjectID tankId);
	static void BroadcastTankHit(ObjectID tankId, ObjectID projectileId, UserDBIndex shooterId, UserDBIndex targetId);
	static void BroadcastCreateObstacle(ObjectID obstacleId, Transform* pTransform);
	static void BroadcastDeleteObstacle(ObjectID obstacleId, UserDBIndex shooterId);
	static void BroadcastRespawnTank(ObjectID tankId);
private:
	static void HandleLogin(BYTE* pGameEvent, SessionID senderId);
	static BOOL ValidateLogin(BYTE* pGameEvent, SessionID senderId);

	static void HandleStartMove(BYTE* pGameEvent, SessionID senderId);
	static BOOL ValidateStartMove(BYTE* pGameEvent, SessionID senderId);

	static void HandleEndMove(BYTE* pGameEvent, SessionID senderId);
	static BOOL ValidateEndMove(BYTE* pGameEvent, SessionID senderId);

	static void HandleMoving(BYTE* pGameEvent, SessionID senderId);
	static BOOL ValidateMoving(BYTE* pGameEvent, SessionID senderId);

	static void HandleShoot(BYTE* pGameEvent, SessionID senderId);
	static BOOL ValidateShoot(BYTE* pGameEvent, SessionID senderId);

	
};


