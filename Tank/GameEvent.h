#pragma once

#include "GameStruct.h"
#include "StaticData.h"
#include "GameMath.h"

const int ID_LENGTH_MAX = 16;
const int PW_LENGTH_MAX = 16;


class GamePacket
{
public:
	static BOOL Validate(BYTE* pGameEvent, UINT32 senderId);
	static void HandlePacket(BYTE* pGameEvent, UINT32 senderId);

	static void SendLogin(const std::wstring& wID, const std::wstring& wPw);
	static void SendStartMove(const Transform* pTankTransform, PlayerInputState moveFlag);
	static void SendEndMove(const Transform* pTankTransform);
	static void SendMoving(const Transform* pTankTransform, PlayerInputState moveFlag);
	static void SendFireMachineGun(const Transform* pTurretTransform);

	static void SetLoginResultCallback(void (*callback)(const PACKET_SC_LOGIN* login));
	static void SetCreateTankCallback(void (*callback)(const PACKET_SC_CREATE_TANK* createTank));
	static void SetDeleteTankCallback(void (*callback)(const PACKET_SC_DELETE_TANK* deleteTank));
	static void SetStartMoveCallback(void (*callback)(const PACKET_SC_START_MOVE* startMove));
	static void SetEndMoveCallback(void (*callback)(const PACKET_SC_END_MOVE* endMove));
	static void SetMovingCallback(void (*callback)(const PACKET_SC_MOVING* moving));
	static void SetSnapshotCallback(void (*callback)(const PACKET_SC_SNAPSHOT* snapshot));
	static void SetFireMachineGunCallback(void (*callback)(const PACKET_SC_FIRE_MACHINE_GUN* fireMachineGun));
	static void SetObjectHitCallback(void (*callback)(const PACKET_SC_OBJECT_HIT* objectHit));
	static void SetRespawnTankCallback(void (*callback)(const PACKET_SC_RESPAWN_TANK* respawnTank));


private:
	static void (*LoginResultCallback)(const PACKET_SC_LOGIN* login);
	static void (*CreateTankCallback)(const PACKET_SC_CREATE_TANK* createTank);
	static void (*DeleteTankCallback)(const PACKET_SC_DELETE_TANK* deleteTank);
	static void (*StartMoveCallback)(const PACKET_SC_START_MOVE* startMove);
	static void (*EndMoveCallback)(const PACKET_SC_END_MOVE* endMove);
	static void (*MovingCallback)(const PACKET_SC_MOVING* moving);
	static void (*SnapshotCallback)(const PACKET_SC_SNAPSHOT* snapshot);
	static void (*FireMachineGunCallback)(const PACKET_SC_FIRE_MACHINE_GUN* fireMachineGun);
	static void (*ObjectHitCallback)(const PACKET_SC_OBJECT_HIT* objectHit);
	static void (*RespawnTankCallback)(const PACKET_SC_RESPAWN_TANK* respawnTank);




	static void DispatchLoginResult(BYTE* pGameEvent, UINT32 senderId);

	static void HandleCreateTank(BYTE* pGameEvent, UINT32 senderId);

	static void HandleDeleteTank(BYTE* pGameEvent, UINT32 senderId);

	static void HandleStartMove(BYTE* pGameEvent, UINT32 senderId);

	static void HandleEndMove(BYTE* pGameEvent, UINT32 senderId);

	static void HandleMoving(BYTE* pGameEvent, UINT32 senderId);

	static void HandleSnapshot(BYTE* pGameEvent, UINT32 senderId);

	static void HandleFireMachineGun(BYTE* pGameEvent, UINT32 senderId);

	static void HandleObjectHit(BYTE* pGameEvent, UINT32 senderID);

	// Deprecated
	static void HandleTankHit(BYTE* pGameEvent, UINT32 senderId);

	static void HandleCreateObstacle(BYTE* pGameEvent, UINT32 senderId);

	static void HandleDeleteObstacle(BYTE* pGameEvent, UINT32 senderId);

	static void HandleRespawnTank(BYTE* pGameEvent, UINT32 senderId);
};
