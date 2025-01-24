#pragma once

enum KeyboardFlagEx : UINT64
{
	INPUT_FLAG_UP = 0b00000001,
	INPUT_FLAG_DOWN = 0b00000010,
	INPUT_FLAG_LEFT = 0b00000100,
	INPUT_FLAG_RIGHT = 0b00001000,
	INPUT_FLAG_ESCAPE = 0b00010000,
	INPUT_FLAG_SPACE = 0b00100000
};

enum EKeyboardMovementStatus
{
	KEYBOARD_MOVEMENT_STATUS_NONE,
	KEYBOARD_MOVEMENT_STATUS_STARTED,
	KEYBOARD_MOVEMENT_STATUS_ENDED,
	KEYBOARD_MOVEMENT_STATUS_MOVING
};

class KeyboardEventListener
{

public:
	static void Initiate();
	static void Terminate();

	static void ResetInputFlag();
	static void ProcessInputs();

	static void GetAndUpdateKeyboardMovementStatus(UINT64* out_pressedKeys, UINT64* out_releasedKeys);


private:
	static UINT64 _crntInputFlag;
	static UINT64 _prevInputFlag;
};

