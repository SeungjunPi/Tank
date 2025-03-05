#pragma once

#include "Player.h"
#include "Global.h"

const ULONGLONG DUMMY_REACTION_SPEED = 300;
const ULONGLONG MOVING_SYNC_INTERVAL = TICK_PER_GAME_FRAME * 30;

class Dummy: public Player
{
public:
	Dummy(std::wstring name, std::wstring pw);

	virtual void OnConnected(SessionID sessionID) override;
	virtual void OnSuccessLogin(UserDBIndex key, Score score) override;

	SessionID ConnectToServer();

	void Tick();


private:
	Vector3 _destinationCoord;

	bool _isConnecting = false;

	PlayerInputState _input = PLAYER_INPUT_NONE;
	PlayerInputState _prevInput = PLAYER_INPUT_NONE;

	ULONGLONG _prevReactionTick = 0;

	void DetermineInput();


};

