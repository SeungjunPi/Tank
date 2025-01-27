#pragma once


#include "Global.h"
#include "NetCoreAliases.h"
#include "TankPch.h"
#include "GameStruct.h"

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
	Tank* GetTank() { return _pTank; }
	Tank* ClearTank();

	void HandleKeyboardEvents(UINT64 pressedKeys, UINT64 releasedKeys, UINT64 heldKeys);

	UserDBIndex GetUserID() const { return _userID; }
	SessionID GetSessionID() const { return _sessionID; }
	
	INT IncreaseHit();
	INT IncreaseKill();
	INT IncreaseDeath();


protected:
	ULONGLONG _prevSyncTick = MAXULONGLONG;

private:
	std::wstring _name;
	std::wstring _password;

	SessionID _sessionID = INVALID_SESSION_ID;
	UserDBIndex _userID = INVALID_USER_ID;

	Tank* _pTank = nullptr;
	Score _score;

	
};