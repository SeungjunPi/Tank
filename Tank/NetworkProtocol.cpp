#include "NetworkProtocol.h"

CSLoginCallback PacketHandler::s_CSLoginCallback = nullptr;
CSStartMoveCallback PacketHandler::s_CSStartMoveCallback = nullptr;
CSEndMoveCallback PacketHandler::s_CSEndMoveCallback = nullptr;
CSMovingCallback PacketHandler::s_CSMovingCallback = nullptr;
CSFireMachineGunCallback PacketHandler::s_CSFireMachineGunCallback = nullptr;

SCLoginCallback PacketHandler::s_SCLoginCallback = nullptr;
SCCreateTankCallback PacketHandler::s_SCCreateTankCallback = nullptr;
SCDeleteTankCallback PacketHandler::s_SCDeleteTankCallback = nullptr;
SCStartMoveCallback PacketHandler::s_SCStartMoveCallback = nullptr;
SCEndMoveCallback PacketHandler::s_SCEndMoveCallback = nullptr;
SCMovingCallback PacketHandler::s_SCMovingCallback = nullptr;
SCSnapshotCallback PacketHandler::s_SCSnapshotCallback = nullptr;
SCFireMachineGunCallback PacketHandler::s_SCFireMachineGunCallback = nullptr;
SCObjectHitCallback PacketHandler::s_SCObjectHitCallback = nullptr;
SCRespawnTankCallback PacketHandler::s_SCRespawnTankCallback = nullptr;
	
BOOL PacketHandler::Validate(BYTE* pGameEvent, UINT32 senderId)
{
	return 0;
}

void PacketHandler::DispatchPacket(BYTE* pGameEvent, UINT32 senderId)
{
	ENetworkMessageType* evCode = (ENetworkMessageType*)pGameEvent;
	switch (*evCode) {
	case GAME_MESSAGE_TYPE_SC_LOGIN:
	{
		PACKET_SC_LOGIN* pScLogin = (PACKET_SC_LOGIN*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCLoginCallback(pScLogin, senderId);
	}
		break;
	case GAME_MESSAGE_TYPE_SC_SNAPSHOT:
	{
		PACKET_SC_SNAPSHOT* pScSnapshot = (PACKET_SC_SNAPSHOT*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCSnapshotCallback(pScSnapshot, senderId);
	}
		break;
	case GAME_MESSAGE_TYPE_SC_CREATE_TANK:
	{
		PACKET_SC_CREATE_TANK* pScCreateTank = (PACKET_SC_CREATE_TANK*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCCreateTankCallback(pScCreateTank, senderId);
	}
		break;
	case GAME_MESSAGE_TYPE_SC_DELETE_TANK:
	{
		PACKET_SC_DELETE_TANK* pScDeleteTank = (PACKET_SC_DELETE_TANK*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCDeleteTankCallback(pScDeleteTank, senderId);
	}
		break;
	case GAME_MESSAGE_TYPE_SC_START_MOVE:
	{
		PACKET_SC_START_MOVE* pScStartMove = (PACKET_SC_START_MOVE*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCStartMoveCallback(pScStartMove, senderId);
	}
		break;
	case GAME_MESSAGE_TYPE_SC_END_MOVE:
	{
		PACKET_SC_END_MOVE* pScEndMove = (PACKET_SC_END_MOVE*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCEndMoveCallback(pScEndMove, senderId);
	}
		break;
	case GAME_MESSAGE_TYPE_SC_MOVING:
	{
		PACKET_SC_MOVING* pScMoving = (PACKET_SC_MOVING*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCMovingCallback(pScMoving, senderId);
	}
		break;
	case GAME_MESSAGE_TYPE_SC_FIRE_MACHINE_GUN:
	{
		PACKET_SC_FIRE_MACHINE_GUN* pScFireMachineGun = (PACKET_SC_FIRE_MACHINE_GUN*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCFireMachineGunCallback(pScFireMachineGun, senderId);
	}
		break;	
	case GAME_MESSAGE_TYPE_SC_RESPAWN_TANK:
	{
		PACKET_SC_RESPAWN_TANK* pScRespawnTank = (PACKET_SC_RESPAWN_TANK*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCRespawnTankCallback(pScRespawnTank, senderId);
	}
		break;
	case GAME_MESSAGE_TYPE_SC_OBJECT_HIT:
	{
		PACKET_SC_OBJECT_HIT* pScObjectHit = (PACKET_SC_OBJECT_HIT*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCObjectHitCallback(pScObjectHit, senderId);
	}
		break;		
	default:
		__debugbreak();
		break;
	}
}

void PacketHandler::RegisterCSLoginCallback(CSLoginCallback callback)
{
	s_CSLoginCallback = callback;
}

void PacketHandler::RegisterCSStartMoveCallback(CSStartMoveCallback callback)
{
	s_CSStartMoveCallback = callback;
	
}

void PacketHandler::RegisterCSEndMoveCallback(CSEndMoveCallback callback)
{
	s_CSEndMoveCallback = callback;
}

void PacketHandler::RegisterCSMovingCallback(CSMovingCallback callback)
{
	s_CSMovingCallback = callback;
}

void PacketHandler::RegisterCSFireMachineGunCallback(CSFireMachineGunCallback callback)
{
	s_CSFireMachineGunCallback = callback;
}

void PacketHandler::RegisterSCLoginCallback(SCLoginCallback callback)
{
	s_SCLoginCallback = callback;
}

void PacketHandler::RegisterSCCreateTankCallback(SCCreateTankCallback callback)
{
	s_SCCreateTankCallback = callback;
}

void PacketHandler::RegisterSCDeleteTankCallback(SCDeleteTankCallback callback)
{
	s_SCDeleteTankCallback = callback;
}

void PacketHandler::RegisterSCStartMoveCallback(SCStartMoveCallback callback)
{
	s_SCStartMoveCallback = callback;
}

void PacketHandler::RegisterSCEndMoveCallback(SCEndMoveCallback callback)
{
	s_SCEndMoveCallback = callback;
}

void PacketHandler::RegisterSCMovingCallback(SCMovingCallback callback)
{
	s_SCMovingCallback = callback;
}

void PacketHandler::RegisterSCSnapshotCallback(SCSnapshotCallback callback)
{
	s_SCSnapshotCallback = callback;
}

void PacketHandler::RegisterSCFireMachineGunCallback(SCFireMachineGunCallback callback)
{
	s_SCFireMachineGunCallback = callback;
}

void PacketHandler::RegisterSCObjectHitCallback(SCObjectHitCallback callback)
{
	s_SCObjectHitCallback = callback;
}

void PacketHandler::RegisterSCRespawnTankCallback(SCRespawnTankCallback callback)
{
	s_SCRespawnTankCallback = callback;
}
