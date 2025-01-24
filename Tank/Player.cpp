#include "Player.h"
#include "Tank.h"


Player::Player(std::wstring name, std::wstring password)
	: _name(name)
	, _password(password)
{
}

Player::~Player()
{
}

void Player::SetSessionID(SessionID sessionID)
{
	if (_sessionID != INVALID_SESSION_ID) {
		__debugbreak();
		return;
	}
	_sessionID = sessionID;
}

void Player::SetUserKey(UserDBIndex key)
{
	if (_userKey != INVALID_USER_KEY) {
		__debugbreak();
		return;
	}
	_userKey = key;
}

void Player::SetTank(Tank* pTank)
{
	if (pTank != nullptr) {
		__debugbreak();
	}
	_pTank = pTank;
}

Tank* Player::ClearTank()
{
	if (_pTank == nullptr) {
		__debugbreak();
	}
	
	Tank* tmp = _pTank;
	_pTank = nullptr;
	return tmp;
}
