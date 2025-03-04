#include "NetworkProtocol.h"

CSLoginCallback PacketHandler::s_CSLoginCallback = nullptr;
CSStartMoveCallback PacketHandler::s_CSStartMoveCallback = nullptr;
CSEndMoveCallback PacketHandler::s_CSEndMoveCallback = nullptr;
CSMovingCallback PacketHandler::s_CSMovingCallback = nullptr;
CSFireMachineGunCallback PacketHandler::s_CSFireMachineGunCallback = nullptr;

CSSendLogin PacketHandler::s_CSSendLogin = nullptr;
CSSendStartMove PacketHandler::s_CSSendStartMove = nullptr;
CSSendEndMove PacketHandler::s_CSSendEndMove = nullptr;
CSSendMoving PacketHandler::s_CSSendMoving = nullptr;
CSSendFireMachineGun PacketHandler::s_CSSendFireMachineGun = nullptr;

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
SCMachineGunCallback PacketHandler::s_SCMachineGunCallback = nullptr;
	
BOOL PacketHandler::Validate(BYTE* pGameEvent, SessionID sessionID)
{
	return 0;
}

void PacketHandler::DispatchPacket(BYTE* pGameEvent, SessionID sessionID)
{
	ENetworkMessageType* evCode = (ENetworkMessageType*)pGameEvent;
	switch (*evCode) {
	// Server -> Client
	case GAME_MESSAGE_TYPE_SC_LOGIN:
	{
		PACKET_SC_LOGIN* pScLogin = (PACKET_SC_LOGIN*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCLoginCallback(pScLogin, sessionID);
	}
	break;
	case GAME_MESSAGE_TYPE_SC_SNAPSHOT:
	{
		PACKET_SC_SNAPSHOT* pScSnapshot = (PACKET_SC_SNAPSHOT*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCSnapshotCallback(pScSnapshot, sessionID);
	}
	break;
	case GAME_MESSAGE_TYPE_SC_CREATE_TANK:
	{
		PACKET_SC_CREATE_TANK* pScCreateTank = (PACKET_SC_CREATE_TANK*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCCreateTankCallback(pScCreateTank, sessionID);
	}
	break;
	case GAME_MESSAGE_TYPE_SC_DELETE_TANK:
	{
		PACKET_SC_DELETE_TANK* pScDeleteTank = (PACKET_SC_DELETE_TANK*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCDeleteTankCallback(pScDeleteTank, sessionID);
	}
	break;
	case GAME_MESSAGE_TYPE_SC_START_MOVE:
	{
		PACKET_SC_START_MOVE* pScStartMove = (PACKET_SC_START_MOVE*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCStartMoveCallback(pScStartMove, sessionID);
	}
	break;
	case GAME_MESSAGE_TYPE_SC_END_MOVE:
	{
		PACKET_SC_END_MOVE* pScEndMove = (PACKET_SC_END_MOVE*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCEndMoveCallback(pScEndMove, sessionID);
	}
	break;
	case GAME_MESSAGE_TYPE_SC_MOVING:
	{
		PACKET_SC_MOVING* pScMoving = (PACKET_SC_MOVING*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCMovingCallback(pScMoving, sessionID);
	}
	break;
	case GAME_MESSAGE_TYPE_SC_FIRE_MACHINE_GUN:
	{
		PACKET_SC_FIRE_MACHINE_GUN* pScFireMachineGun = (PACKET_SC_FIRE_MACHINE_GUN*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCFireMachineGunCallback(pScFireMachineGun, sessionID);
	}
	break;
	case GAME_MESSAGE_TYPE_SC_RESPAWN_TANK:
	{
		PACKET_SC_RESPAWN_TANK* pScRespawnTank = (PACKET_SC_RESPAWN_TANK*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCRespawnTankCallback(pScRespawnTank, sessionID);
	}
	break;
	case GAME_MESSAGE_TYPE_SC_OBJECT_HIT:
	{
		PACKET_SC_OBJECT_HIT* pScObjectHit = (PACKET_SC_OBJECT_HIT*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCObjectHitCallback(pScObjectHit, sessionID);
	}
	break;
	case GAME_MESSAGE_TYPE_SC_MACHINE_GUN_HIT:
	{
		PACKET_SC_MACHINE_GUN_HIT* pScMachineGunHit = (PACKET_SC_MACHINE_GUN_HIT*)(pGameEvent + sizeof(ENetworkMessageType));
		s_SCMachineGunCallback(pScMachineGunHit, sessionID);
	}
	break;
	
	// Client -> Server
	case GAME_MESSAGE_TYPE_CS_LOGIN:
	{
		PACKET_CS_LOGIN* pCsLogin = (PACKET_CS_LOGIN*)(pGameEvent + sizeof(ENetworkMessageType));
		s_CSLoginCallback(pCsLogin, sessionID);
	}
	break;
	case GAME_MESSAGE_TYPE_CS_START_MOVE:
	{
		PACKET_CS_START_MOVE* pCsStartMove = (PACKET_CS_START_MOVE*)(pGameEvent + sizeof(ENetworkMessageType));
		s_CSStartMoveCallback(pCsStartMove, sessionID);
	}
	break;
	case GAME_MESSAGE_TYPE_CS_END_MOVE:
	{
		PACKET_CS_END_MOVE* pCsEndMove = (PACKET_CS_END_MOVE*)(pGameEvent + sizeof(ENetworkMessageType));
		s_CSEndMoveCallback(pCsEndMove, sessionID);
	}
	break;
	case GAME_MESSAGE_TYPE_CS_MOVING:
	{
		PACKET_CS_MOVING* pCsMoving = (PACKET_CS_MOVING*)(pGameEvent + sizeof(ENetworkMessageType));
		s_CSMovingCallback(pCsMoving, sessionID);
	}
	break;
	case GAME_MESSAGE_TYPE_CS_FIRE_MACHINE_GUN:
	{
		PACKET_CS_FIRE_MACHINE_GUN* pCsFireMachineGun = (PACKET_CS_FIRE_MACHINE_GUN*)(pGameEvent + sizeof(ENetworkMessageType));
		s_CSFireMachineGunCallback(pCsFireMachineGun, sessionID);
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

void PacketHandler::RegisterCSSendLogin(CSSendLogin send)
{
	s_CSSendLogin = send;
}

void PacketHandler::RegisterCSSendStartMove(CSSendStartMove send)
{
	s_CSSendStartMove = send;
}

void PacketHandler::RegisterCSSendEndMove(CSSendEndMove send)
{
	s_CSSendEndMove = send;
}

void PacketHandler::RegisterCSSendMoving(CSSendMoving send)
{
	s_CSSendMoving = send;
}

void PacketHandler::RegisterCSSendFireMachineGun(CSSendFireMachineGun send)
{
	s_CSSendFireMachineGun = send;
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

void PacketHandler::RegisterSCMachineGunHitCallback(SCMachineGunCallback callback)
{
	s_SCMachineGunCallback = callback;
}
