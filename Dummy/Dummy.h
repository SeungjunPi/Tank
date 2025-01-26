#pragma once

#include "Player.h"

const ULONGLONG DUMMY_REACTION_SPEED = 300;

class Dummy : public Player
{
public:
	Dummy(std::wstring name, std::wstring pw);

	virtual void OnConnected(SessionID sessionID) override;
	virtual void OnSuccessLogin(UserDBIndex key, Score score) override;

	void OnFrame();
	

private:
	Transform* _transform = nullptr;
	Vector3 _destinationCoord;

	UINT64 _virtualInputEvent = 0;
	UINT64 _prevInputEvent = 0;

	bool _isConnecting = false;

	ULONGLONG _prevTick = 0;

	void DetermineInput();

	
};

