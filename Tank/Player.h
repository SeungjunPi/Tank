#pragma once



#include "NetCoreAliases.h"
#include "TankPch.h"

class Tank;

class Player
{
public:
	Player(std::wstring name, std::wstring password);
	~Player();

	void SetSessionID(SessionID sessionID);
	void SetUserKey(UserDBIndex key);

	std::wstring& GetName() { return _name; }
	std::wstring& GetPassword() { return _password; }

	void SetTank(Tank* pTank);
	Tank* ClearTank();

	void OnPressUp();
	void OnReleaseUp();

	void OnPressDown();
	void OnReleaseDown();



	

private:
	std::wstring _name;
	std::wstring _password;

	SessionID _sessionID = INVALID_SESSION_ID;
	UserDBIndex _userKey = INVALID_USER_KEY;

	Tank* _pTank = nullptr;

	// LastSyncTick..?
};