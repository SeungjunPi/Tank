#pragma once

#include "SentinelPch.h"
#include "SentinelAliases.h"
#include "GameStruct.h"
#include "StaticData.h"

const int ID_LENGTH_MAX = 16;
const int PW_LENGTH_MAX = 16;

// [Kind3][Kind2][Kind1][SC, CS] 
enum EGameEventCode
{
	GAME_EVENT_CODE_CS_LOGIN,
	GAME_EVENT_CODE_SC_LOGIN,
	GAME_EVENT_CODE_CS_LOAD_PLAYER_STAT,
	GAME_EVENT_CODE_SC_LOAD_PLAYER_STAT,
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
	GAME_EVENT_CODE_CS_FIRE_MACHINE_GUN = 0x02010401,
	GAME_EVENT_CODE_SC_FIRE_MACHINE_GUN = 0x02010402,
	GAME_EVENT_CODE_SC_OBJECT_HIT = 0x02010501,
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
	PlayerInputState inputState;
};

struct PACKET_SC_DELETE_TANK
{
	ObjectID objectId;
};

struct PACKET_CS_START_MOVE
{
	PlayerInputState inputState;
};

struct PACKET_SC_START_MOVE
{
	PlayerInputState inputState;
	ObjectID objectId;
	Transform transform; // 클라이언트가 확인하기 위한 용도
};

struct PACKET_CS_END_MOVE
{
	Transform transform; // 보정용
};

struct PACKET_SC_END_MOVE
{
	ObjectID objectId;
	Transform transform;
};

struct PACKET_CS_MOVING
{
	PlayerInputState inputState;
	Transform transform;
};

struct PACKET_SC_MOVING
{
	PlayerInputState inputState;
	ObjectID objectId;
	Transform transform;
};

struct PACKET_OBJECT_INFO
{
	EGameObjectType kind;
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

struct PACKET_SC_OBJECT_HIT
{
	ObjectID oneID;
	ObjectID anotherID;
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
	static void BroadcastDeleteTank(ObjectID tankId);

	// Deprecated
	static void BroadcastTankHit(ObjectID tankId, ObjectID projectileId, UserDBIndex shooterId, UserDBIndex targetId);

	// Deprecated
	static void BroadcastHit(ObjectID objectID, UserDBIndex ownerID, UINT32 kindnessFlag);
	
	static void BroadcastHit(ObjectID oneID, ObjectID anotherID);
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

	static void HandleFireMachineGun(BYTE* pGameEvent, SessionID senderId);
	static BOOL ValidateFireMachineGun(BYTE* pGameEvent, SessionID senderId);

	
};


