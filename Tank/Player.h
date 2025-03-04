#pragma once



#include "NetCoreAliases.h"
#include "TankPch.h"
#include "GameStruct.h"
#include "NetworkAlias.h"
#include "Global.h"



struct Score;
class Tank;

class Player
{
public:
	Player(std::wstring name, std::wstring password);
	~Player();

	virtual void OnConnected(SessionID sessionID);
	virtual void OnSuccessLogin(UserDBIndex key, Score score);

	std::wstring& GetName() { return _name; }
	std::wstring& GetPassword() { return _password; }

	void SetTank(Tank* pTank);
	ObjectID GetTankID();
	Tank* GetTankPtr();
	Tank* ClearTank();

	void HandleKeyboardEvents(UINT64 pressedKeys, UINT64 releasedKeys, UINT64 heldKeys);

	UserDBIndex GetUserID() const { return _userID; }
	SessionID GetSessionID() const { return _sessionID; }
	
	INT IncreaseHit();
	INT IncreaseKill();
	INT IncreaseDeath();

	void Tick();

	void LogTankPosition(const char* str);
protected:
	PlayerInputState _crntInputState = PLAYER_INPUT_NONE;
	PlayerInputState _prevInputState = PLAYER_INPUT_NONE;

	ULONGLONG _lastMovementSyncTick = 0;

private:
	std::wstring _name;
	std::wstring _password;

	SessionID _sessionID = INVALID_SESSION_ID;
	UserDBIndex _userID = INVALID_USER_ID;

	Tank* _pTank = nullptr;
	Score _score;

	void UpdateInputState(UINT64 heldKeys);
};