#pragma once

#include "NetworkProtocol.h"

class ClientPacketHandler
{
public:
	static void RegisterCallbacks();

	static void SendLogin(const std::wstring& wID, const std::wstring& wPw);
	static void SendStartMove(const Transform* pTankTransform, PlayerInputState inputState);
	static void SendEndMove(const Transform* pTankTransform);
	static void SendMoving(const Transform* pTankTransform, PlayerInputState inputState);
	static void SendFireMachineGun(const Transform* pTurretTransform);

private:
	static void OnLogin(const PACKET_SC_LOGIN* login, UINT32 senderID);
	static void OnCreateTank(const PACKET_SC_CREATE_TANK* createTank, UINT32 senderID);
	static void OnDeleteTank(const PACKET_SC_DELETE_TANK* deleteTank, UINT32 senderID);
	static void OnStartMove(const PACKET_SC_START_MOVE* startMove, UINT32 senderID);
	static void OnEndMove(const PACKET_SC_END_MOVE* endMove, UINT32 senderID);
	static void OnMoving(const PACKET_SC_MOVING* moving, UINT32 senderID);
	static void OnSnapshot(const PACKET_SC_SNAPSHOT* snapshot, UINT32 senderID);
	static void OnFireMachineGun(const PACKET_SC_FIRE_MACHINE_GUN* fireMachineGun, UINT32 senderID);
	static void OnObjectHit(const PACKET_SC_OBJECT_HIT* objectHit, UINT32 senderID);
	static void OnRespawnTank(const PACKET_SC_RESPAWN_TANK* respawnTank, UINT32 senderID);


};

