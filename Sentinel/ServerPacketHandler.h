#pragma once


#include "SentinelPch.h"
#include "NetworkProtocol.h"
#include "NetCoreAliases.h"

class ServerPacketHandler
{
public:
	static void RegisterCallbacks();;

	static void SendLogin(SessionID sessionId, BOOL result, UserDBIndex userDBIndex, int hitCount, int killCount, int deathCount);
	static void SendSnapshot(SessionID sessionId);
	static void BroadcastCreateTank(ObjectID tankId, UserDBIndex userIndex, const Transform* pTransform);
	static void BroadcastDeleteTank(ObjectID tankId);

	static void BroadcastHit(ObjectID oneID, ObjectID anotherID);
	static void BroadcastCreateObstacle(ObjectID obstacleId, Transform* pTransform);
	static void BroadcastDeleteObstacle(ObjectID obstacleId, UserDBIndex shooterId);
	static void BroadcastRespawnTank(ObjectID tankId);

private:
	static void OnLogin(const PACKET_CS_LOGIN* login, UINT32 senderID);
	static void OnStartMove(const PACKET_CS_START_MOVE* startMove, UINT32 senderID);
	static void OnEndMove(const PACKET_CS_END_MOVE* endMove, UINT32 senderID);
	static void OnMoving(const PACKET_CS_MOVING* moving, UINT32 senderID);
	static void OnFireMachineGun(const PACKET_CS_FIRE_MACHINE_GUN* fireMachineGun, UINT32 senderID);
};
