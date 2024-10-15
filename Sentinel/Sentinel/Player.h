#pragma once

#include "stdafx.h"

class Player
{
public:
	Player(UINT32 sessionId);
	UINT32 GetSessionId() const;
	UINT32 GetChatObjId() const;
	void SetChatObjId(UINT32 chatObjId);

	UINT32 GetTankId() const;
	void SetTankId(UINT32 tankId);
private:
	UINT32 _sessionId = 0;
	UINT32 _chatObjId = 0;
	UINT32 _tankId = 0;
};

