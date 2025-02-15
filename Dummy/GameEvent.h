﻿#pragma once

#include "GameStruct.h"
#include "StaticData.h"
#include "GameMath.h"

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
	static BOOL Validate(BYTE* pGameEvent, UINT32 senderId);
	static void HandlePacket(BYTE* pGameEvent, UINT32 senderId);

	static void SendLogin(const std::wstring& wID, const std::wstring& wPw, UINT32 sessionID);
	static void SendStartMove(const Transform* pTankTransform, PlayerInputState moveFlag, UINT32 sessionID);
	static void SendEndMove(const Transform* pTankTransform, UINT32 sessionID);
	static void SendMoving(const Transform* pTankTransform, PlayerInputState moveFlag, UINT32 sessionID);
	static void SendFireMachineGun(const Transform* pTurretTransform, UINT32 sessionID);


private:
	static void HandleLoginResult(BYTE* pGameEvent, UINT32 sessionID);

	static void HandleCreateTank(BYTE* pGameEvent, UINT32 sessionID);

	static void HandleDeleteTank(BYTE* pGameEvent, UINT32 sessionID);

	static void HandleStartMove(BYTE* pGameEvent, UINT32 sessionID);

	static void HandleEndMove(BYTE* pGameEvent, UINT32 sessionID);

	static void HandleMoving(BYTE* pGameEvent, UINT32 sessionID);

	static void HandleSnapshot(BYTE* pGameEvent, UINT32 sessionID);

	static void HandleFireMachineGun(BYTE* pGameEvent, UINT32 sessionID);
	
	static void HandleObjectHit(BYTE* pGameEvent, UINT32 sessionID);

	// Deprecated
	static void HandleTankHit(BYTE* pGameEvent, UINT32 sessionID);

	static void HandleCreateObstacle(BYTE* pGameEvent, UINT32 sessionID);

	static void HandleDeleteObstacle(BYTE* pGameEvent, UINT32 sessionID);

	static void HandleRespawnTank(BYTE* pGameEvent, UINT32 sessionID);
};
