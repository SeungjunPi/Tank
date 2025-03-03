#pragma once

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


struct PACKET_CS_FIRE_MACHINE_GUN
{
	Transform transform;
};

struct PACKET_SC_FIRE_MACHINE_GUN
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


class IPacketCallbacks
{
public:
	virtual ~IPacketCallbacks() = default;

	// Client -> Server packets
	virtual void OnLogin(const PACKET_CS_LOGIN* login, UINT32 senderId) {}
	virtual void OnStartMove(const PACKET_CS_START_MOVE* startMove, UINT32 senderId) {}
	virtual void OnEndMove(const PACKET_CS_END_MOVE* endMove, UINT32 senderId) {}
	virtual void OnMoving(const PACKET_CS_MOVING* moving, UINT32 senderId) {}
	virtual void OnFireMachineGun(const PACKET_CS_FIRE_MACHINE_GUN* fireMachineGun, UINT32 senderId) {}

	// Server -> Client packets
	virtual void OnLoginResult(const PACKET_SC_LOGIN* login, UINT32 senderId) {}
	virtual void OnCreateTank(const PACKET_SC_CREATE_TANK* createTank, UINT32 senderId) {}
	virtual void OnDeleteTank(const PACKET_SC_DELETE_TANK* deleteTank, UINT32 senderId) {}
	virtual void OnStartMoveResult(const PACKET_SC_START_MOVE* startMove, UINT32 senderId) {}
	virtual void OnEndMoveResult(const PACKET_SC_END_MOVE* endMove, UINT32 senderId) {}
	virtual void OnMovingResult(const PACKET_SC_MOVING* moving, UINT32 senderId) {}
	virtual void OnSnapshot(const PACKET_SC_SNAPSHOT* snapshot, UINT32 senderId) {}
	virtual void OnFireMachineGunResult(const PACKET_SC_FIRE_MACHINE_GUN* fireMachineGun, UINT32 senderId) {}
	virtual void OnObjectHit(const PACKET_SC_OBJECT_HIT* objectHit, UINT32 senderId) {}
	virtual void OnRespawnTank(const PACKET_SC_RESPAWN_TANK* respawnTank, UINT32 senderId) {}
};


class PacketHandler
{
public:
	static void RegisterCallbacks(IPacketCallbacks* callbacks);

	static BOOL Validate(BYTE* pGameEvent, UINT32 senderId);
	static void HandlePacket(BYTE* pGameEvent, UINT32 senderId);

private:
	static IPacketCallbacks* s_callbacks;

	static void DispatchLoginRequest(BYTE* pPacket, UINT32 senderId);
	static void DispatchLoginResult(BYTE* pPacket, UINT32 senderId);
	static void DispatchCreateTank(BYTE* pPacket, UINT32 senderId);
	static void DispatchDeleteTank(BYTE* pPacket, UINT32 senderId);
	static void DispatchStartMove(BYTE* pPacket, UINT32 senderId);
	static void DispatchEndMove(BYTE* pPacket, UINT32 senderId);
	static void DispatchMoving(BYTE* pPacket, UINT32 senderId);
	static void DispatchSnapshot(BYTE* pPacket, UINT32 senderId);
	static void DispatchFireMachineGun(BYTE* pPacket, UINT32 senderId);
	static void DispatchObjectHit(BYTE* pPacket, UINT32 senderId);
	static void DispatchRespawnTank(BYTE* pPacket, UINT32 senderId);
};
