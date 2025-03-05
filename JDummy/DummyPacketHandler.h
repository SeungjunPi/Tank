#pragma once

#include "NetworkProtocol.h"
#include "NetCoreAliases.h"


class DummyPacketHandler
{
public:
	static void RegisterCallbacks();

	static void SendLogin(const std::wstring& wID, const std::wstring& wPw, SessionID sessionID);
	static void SendStartMove(const Transform* pTankTransform, PlayerInputState inputState, SessionID sessionID);
	static void SendEndMove(const Transform* pTankTransform, SessionID sessionID);
	static void SendMoving(const Transform* pTankTransform, PlayerInputState inputState, SessionID sessionID);
	static void SendFireMachineGun(const Transform* pTurretTransform, SessionID sessionID);

private:
	static void OnLogin(const PACKET_SC_LOGIN* login, UINT32 sessionID);
	static void OnCreateTank(const PACKET_SC_CREATE_TANK* createTank, UINT32 sessionID);
	static void OnDeleteTank(const PACKET_SC_DELETE_TANK* deleteTank, UINT32 sessionID);
	static void OnStartMove(const PACKET_SC_START_MOVE* startMove, UINT32 sessionID);
	static void OnEndMove(const PACKET_SC_END_MOVE* endMove, UINT32 sessionID);
	static void OnMoving(const PACKET_SC_MOVING* moving, UINT32 sessionID);
	static void OnSnapshot(const PACKET_SC_SNAPSHOT* snapshot, UINT32 sessionID);
	static void OnFireMachineGun(const PACKET_SC_FIRE_MACHINE_GUN* fireMachineGun, UINT32 sessionID);
	static void OnObjectHit(const PACKET_SC_OBJECT_HIT* objectHit, UINT32 sessionID);
	static void OnRespawnTank(const PACKET_SC_RESPAWN_TANK* respawnTank, UINT32 sessionID);
	static void OnMachineGunHit(const PACKET_SC_MACHINE_GUN_HIT* machineGunHit, UINT32 sessionID);

};
