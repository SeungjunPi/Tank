#pragma once

enum EKeyboardFlagEx : UINT64
{
	KEYBOARD_INPUT_FLAG_UP		= 0b00000001,
	KEYBOARD_INPUT_FLAG_DOWN	= 0b00000010,
	KEYBOARD_INPUT_FLAG_LEFT	= 0b00000100,
	KEYBOARD_INPUT_FLAG_RIGHT	= 0b00001000,
	KEYBOARD_INPUT_FLAG_ESC		= 0b00010000,
	KEYBOARD_INPUT_FLAG_SPACE	= 0b00100000
};

class KeyboardEventListener
{

public:
	static void Initiate();
	static void Terminate();

	static void ResetInputFlag();
	static void ProcessInputs();

	static void GetAndUpdateKeyboardMovementStatus(UINT64* out_pressedKeys, UINT64* out_releasedKeys, UINT64* out_heldKeys);


private:
	static UINT64 _crntInputFlag;
	static UINT64 _prevInputFlag;
};

