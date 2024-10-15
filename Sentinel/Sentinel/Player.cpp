#include "Player.h"

Player::Player(UINT32 sessionId)
	: _sessionId(sessionId)
{
}

UINT32 Player::GetSessionId() const
{
	return _sessionId;
}

UINT32 Player::GetChatObjId() const
{
	return _chatObjId;
}

void Player::SetChatObjId(UINT32 chatObjId)
{
	_chatObjId = chatObjId;
}

UINT32 Player::GetTankId() const
{
	return _tankId;
}

void Player::SetTankId(UINT32 tankId)
{
	_tankId = tankId;
}
