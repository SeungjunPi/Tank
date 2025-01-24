#include <memory>

#include <windows.h>
#include <conio.h>
#include <cstdio>
#include <thread>
#include "KeyboardEventListener.h"

UINT64 KeyboardEventListener::_crntInputFlag = 0;
UINT64 KeyboardEventListener::_prevInputFlag = 0;




static HWND s_gameWindowHandle = nullptr;


void KeyboardEventListener::Initiate()
{
	s_gameWindowHandle = GetForegroundWindow();
}

void KeyboardEventListener::Terminate()
{
}

void KeyboardEventListener::ResetInputFlag()
{
	_prevInputFlag = _crntInputFlag;
	_crntInputFlag = 0;
}

void KeyboardEventListener::ProcessInputs()
{
	HWND currentWindowHandle = GetForegroundWindow();
	if (currentWindowHandle != s_gameWindowHandle) {
		// Todo: 윈도우 비활성화 시 모든 입력을 비활성화하게 변경, 필요시 OnKeyUp을 호출하게 변경 
		return;
	}

	

	if (GetAsyncKeyState(VK_UP)) {
		_crntInputFlag |= KeyboardFlagEx::INPUT_FLAG_UP;
	}
	else {
		_crntInputFlag &= (~KeyboardFlagEx::INPUT_FLAG_UP);
	}

	if (GetAsyncKeyState(VK_DOWN)) {
		_crntInputFlag |= KeyboardFlagEx::INPUT_FLAG_DOWN;
	}
	else {
		_crntInputFlag &= (~KeyboardFlagEx::INPUT_FLAG_DOWN);
	}

	if (GetAsyncKeyState(VK_LEFT)) {
		_crntInputFlag |= KeyboardFlagEx::INPUT_FLAG_LEFT;
	}
	else {
		_crntInputFlag &= (~KeyboardFlagEx::INPUT_FLAG_LEFT);
	}

	if (GetAsyncKeyState(VK_RIGHT)) {
		_crntInputFlag |= KeyboardFlagEx::INPUT_FLAG_LEFT;
	}
	else {
		_crntInputFlag &= (~KeyboardFlagEx::INPUT_FLAG_LEFT);
	}

	if (GetAsyncKeyState('I') || GetAsyncKeyState('i')) {
		
	}
	else {
		
	}

	if (GetAsyncKeyState('K') || GetAsyncKeyState('k')) {
		
	}
	else {
		
	}

	if (GetAsyncKeyState(VK_ESCAPE)) {
		_crntInputFlag |= KeyboardFlagEx::INPUT_FLAG_ESCAPE;
	}
	else {
		_crntInputFlag &= (~KeyboardFlagEx::INPUT_FLAG_ESCAPE);
	}

	if (GetAsyncKeyState(VK_SPACE)) {
		_crntInputFlag |= KeyboardFlagEx::INPUT_FLAG_SPACE;
	}
	else {
		_crntInputFlag &= (~KeyboardFlagEx::INPUT_FLAG_SPACE);
	}
	return;
}

void KeyboardEventListener::GetAndUpdateKeyboardMovementStatus(UINT64* out_pressedKeys, UINT64* out_releasedKeys)
{
	UINT64 edgeTrigger = _crntInputFlag ^ _prevInputFlag;

	*out_pressedKeys = edgeTrigger & _crntInputFlag;
	*out_releasedKeys = edgeTrigger & (~_crntInputFlag);

	ResetInputFlag();
}
