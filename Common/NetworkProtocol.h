#pragma once

#include "pch.h"

#include "GameStruct.h"

#include "NetworkAlias.h"


const int ID_LENGTH_MAX = 16;
const int PW_LENGTH_MAX = 16;


// [Kind3][Kind2][Kind1][SC, CS] 
enum ENetworkMessageType
{
	GAME_MESSAGE_TYPE_CS_LOGIN,
	GAME_MESSAGE_TYPE_SC_LOGIN,
	GAME_MESSAGE_TYPE_CS_LOAD_PLAYER_STAT,
	GAME_MESSAGE_TYPE_SC_LOAD_PLAYER_STAT,
	GAME_MESSAGE_TYPE_SC_SNAPSHOT = 0x00010202,
	GAME_MESSAGE_TYPE_SC_CREATE_TANK = 0x01010102,
	GAME_MESSAGE_TYPE_SC_DELETE_TANK = 0x01010202,
	GAME_MESSAGE_TYPE_SC_RESPAWN_TANK = 0x01010302,
	GAME_MESSAGE_TYPE_CS_START_MOVE = 0x02010101,
	GAME_MESSAGE_TYPE_SC_START_MOVE = 0x02010102,
	GAME_MESSAGE_TYPE_CS_END_MOVE = 0x02010201,
	GAME_MESSAGE_TYPE_SC_END_MOVE = 0x02010202,
	GAME_MESSAGE_TYPE_CS_MOVING = 0x02010301,
	GAME_MESSAGE_TYPE_SC_MOVING = 0x02010302,
	GAME_MESSAGE_TYPE_CS_FIRE_MACHINE_GUN = 0x02010401,
	GAME_MESSAGE_TYPE_SC_FIRE_MACHINE_GUN = 0x02010402,
	GAME_MESSAGE_TYPE_SC_OBJECT_HIT = 0x02010501,
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

// Client -> Server
typedef void (*CSLoginCallback)(const PACKET_CS_LOGIN* login, UINT32 senderId);
typedef void (*CSStartMoveCallback)(const PACKET_CS_START_MOVE* startMove, UINT32 senderId);
typedef void (*CSEndMoveCallback)(const PACKET_CS_END_MOVE* endMove, UINT32 senderId);
typedef void (*CSMovingCallback)(const PACKET_CS_MOVING* moving, UINT32 senderId);
typedef void (*CSFireMachineGunCallback)(const PACKET_CS_FIRE_MACHINE_GUN* fireMachineGun, UINT32 senderId);

// Server -> Client
typedef void (*SCLoginCallback)(const PACKET_SC_LOGIN* login, UINT32 senderId);
typedef void (*SCCreateTankCallback)(const PACKET_SC_CREATE_TANK* createTank, UINT32 senderId);
typedef void (*SCDeleteTankCallback)(const PACKET_SC_DELETE_TANK* deleteTank, UINT32 senderId);
typedef void (*SCStartMoveCallback)(const PACKET_SC_START_MOVE* startMove, UINT32 senderId);
typedef void (*SCEndMoveCallback)(const PACKET_SC_END_MOVE* endMove, UINT32 senderId);
typedef void (*SCMovingCallback)(const PACKET_SC_MOVING* moving, UINT32 senderId);
typedef void (*SCSnapshotCallback)(const PACKET_SC_SNAPSHOT* snapshot, UINT32 senderId);
typedef void (*SCFireMachineGunCallback)(const PACKET_SC_FIRE_MACHINE_GUN* fireMachineGun, UINT32 senderId);
typedef void (*SCObjectHitCallback)(const PACKET_SC_OBJECT_HIT* objectHit, UINT32 senderId);
typedef void (*SCRespawnTankCallback)(const PACKET_SC_RESPAWN_TANK* respawnTank, UINT32 senderId);

class PacketHandler
{
public:
	static BOOL Validate(BYTE* pGameEvent, UINT32 senderId);
	static void DispatchPacket(BYTE* pGameEvent, UINT32 senderId);

	static void RegisterCSLoginCallback(CSLoginCallback callback);
	static void RegisterCSStartMoveCallback(CSStartMoveCallback callback);
	static void RegisterCSEndMoveCallback(CSEndMoveCallback callback);
	static void RegisterCSMovingCallback(CSMovingCallback callback);
	static void RegisterCSFireMachineGunCallback(CSFireMachineGunCallback callback);
	
	static void RegisterSCLoginCallback(SCLoginCallback callback);
	static void RegisterSCCreateTankCallback(SCCreateTankCallback callback);
	static void RegisterSCDeleteTankCallback(SCDeleteTankCallback callback);
	static void RegisterSCStartMoveCallback(SCStartMoveCallback callback);
	static void RegisterSCEndMoveCallback(SCEndMoveCallback callback);
	static void RegisterSCMovingCallback(SCMovingCallback callback);
	static void RegisterSCSnapshotCallback(SCSnapshotCallback callback);
	static void RegisterSCFireMachineGunCallback(SCFireMachineGunCallback callback);
	static void RegisterSCObjectHitCallback(SCObjectHitCallback callback);
	static void RegisterSCRespawnTankCallback(SCRespawnTankCallback callback);

private:
	static CSLoginCallback s_CSLoginCallback;
	static CSStartMoveCallback s_CSStartMoveCallback;
	static CSEndMoveCallback s_CSEndMoveCallback;
	static CSMovingCallback s_CSMovingCallback;
	static CSFireMachineGunCallback s_CSFireMachineGunCallback;

	static SCLoginCallback s_SCLoginCallback;
	static SCCreateTankCallback s_SCCreateTankCallback;
	static SCDeleteTankCallback s_SCDeleteTankCallback;
	static SCStartMoveCallback s_SCStartMoveCallback;
	static SCEndMoveCallback s_SCEndMoveCallback;
	static SCMovingCallback s_SCMovingCallback;
	static SCSnapshotCallback s_SCSnapshotCallback;
	static SCFireMachineGunCallback s_SCFireMachineGunCallback;
	static SCObjectHitCallback s_SCObjectHitCallback;
	static SCRespawnTankCallback s_SCRespawnTankCallback;
};
