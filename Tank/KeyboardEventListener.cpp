#include <memory>

#include <windows.h>
#include <conio.h>
#include <cstdio>
#include <thread>
#include "KeyboardEventListener.h"


KeyboardInputs KeyboardEventListener::inputs = { 0, };
char KeyboardEventListener::_movingFlag = 0;
char KeyboardEventListener::_prevMovingFlag = 0;


static const char s_wFlag = 0b00000001;
static const char s_sFlag = 0b00000010;
static const char s_aFlag = 0b00000100;
static const char s_dFlag = 0b00001000;


void KeyboardEventListener::Initiate()
{
	ZeroMemory(&inputs, sizeof(KeyboardInputs));
	inputs.escape = false;
}

void KeyboardEventListener::Terminate()
{
}

void KeyboardEventListener::ProcessInputs()
{
	if (GetAsyncKeyState('W') || GetAsyncKeyState('w')) {
		if (!inputs.up) {
			OnKeyDown_W();
		}
		inputs.up = true;
	}
	else {
		if (inputs.up) {
			OnKeyUp_W();
			inputs.up = false;
		}
	}

	if (GetAsyncKeyState('S') || GetAsyncKeyState('s')) {
		if (!inputs.down) {
			OnKeyDown_S();
		}
		inputs.down = true;
	}
	else {
		if (inputs.down) {
			OnKeyUp_S();
			inputs.down = false;
		}
	}

	if (GetAsyncKeyState('A') || GetAsyncKeyState('a')) {
		if (!inputs.left) {
			OnKeyDown_A();
		}
		inputs.left = true;
	}
	else {
		if (inputs.left) {
			OnKeyUp_A();
			inputs.left = false;
		}
	}

	if (GetAsyncKeyState('D') || GetAsyncKeyState('d')) {
		if (!inputs.right) {
			OnKeyDown_D();
		}
		inputs.right = true;
	}
	else {
		if (inputs.right) {
			OnKeyUp_D();
			inputs.right = false;
		}
	}

	if (GetAsyncKeyState('I') || GetAsyncKeyState('i')) {
		if (!inputs.spawn) {

		}
		inputs.spawn = true;
	}
	else {
		if (inputs.spawn) {
			inputs.spawn = false;
		}
	}

	if (GetAsyncKeyState('K') || GetAsyncKeyState('k')) {
		if (!inputs.despawn) {

		}
		inputs.despawn = true;
	}
	else {
		if (inputs.despawn) {
			inputs.despawn = false;
		}
	}

	if (GetAsyncKeyState(VK_ESCAPE)) {
		inputs.escape = true;
	}
	else {
		inputs.escape = false;
	}

	if (GetAsyncKeyState(VK_SPACE)) {
		inputs.shoot = true;
	}
	else {
		inputs.shoot = false;
	}

	return;
}

EKeyboardMovementStatus KeyboardEventListener::GetAndUpdateKeyboardMovingStatus()
{
	if (!(_movingFlag ^ _prevMovingFlag)) {
		if (_movingFlag != 0) {
			return KEYBOARD_MOVEMENT_STATUS_MOVING;
		}
		return KEYBOARD_MOVEMENT_STATUS_NONE;
	}
	
	if (_movingFlag != 0 && _prevMovingFlag != 0) {
		_prevMovingFlag = _movingFlag;
		return KEYBOARD_MOVEMENT_STATUS_MOVING;
	}

	_prevMovingFlag = _movingFlag;
	if (_movingFlag == 0) {
		return KEYBOARD_MOVEMENT_STATUS_ENDED;
	}

	return KEYBOARD_MOVEMENT_STATUS_STARTED;
}

void KeyboardEventListener::OnKeyDown_W()
{
	KeyboardEventListener::_movingFlag = KeyboardEventListener::_movingFlag | s_wFlag;
}

void KeyboardEventListener::OnKeyUp_W()
{
	KeyboardEventListener::_movingFlag = KeyboardEventListener::_movingFlag & (~s_wFlag);
}

void KeyboardEventListener::OnKeyDown_S()
{
	KeyboardEventListener::_movingFlag = KeyboardEventListener::_movingFlag | s_sFlag;
}

void KeyboardEventListener::OnKeyUp_S()
{
	KeyboardEventListener::_movingFlag = KeyboardEventListener::_movingFlag & (~s_sFlag);
}

void KeyboardEventListener::OnKeyDown_A()
{
	KeyboardEventListener::_movingFlag = KeyboardEventListener::_movingFlag | s_aFlag;
}

void KeyboardEventListener::OnKeyUp_A()
{
	KeyboardEventListener::_movingFlag = KeyboardEventListener::_movingFlag & (~s_aFlag);
}

void KeyboardEventListener::OnKeyDown_D()
{
	KeyboardEventListener::_movingFlag = KeyboardEventListener::_movingFlag | s_dFlag;
}

void KeyboardEventListener::OnKeyUp_D()
{
	KeyboardEventListener::_movingFlag = KeyboardEventListener::_movingFlag & (~s_dFlag);
}
