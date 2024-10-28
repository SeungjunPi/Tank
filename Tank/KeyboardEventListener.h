#pragma once

enum KeyboardFlag : char
{
	KEYBOARD_INPUT_FLAG_W = 0b00000001,
	KEYBOARD_INPUT_FLAG_S = 0b00000010,
	KEYBOARD_INPUT_FLAG_A = 0b00000100,
	KEYBOARD_INPUT_FLAG_D = 0b00001000
};

struct KeyboardInputs
{
	bool up;
	bool down;
	bool left;
	bool right;
	bool escape;
	bool shoot;
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

	static void ProcessInputs();

	static KeyboardInputs inputs;

	static EKeyboardMovementStatus GetAndUpdateKeyboardMovingStatus(); // deprecated
	static void ResetKeyboardMovementStatus();
	static void GetAndUpdateKeyboardMovementStatus(char* out_startFlag, char* out_endFlag, char* out_movingFlag);


private:
	static void OnKeyDown_W();
	static void OnKeyUp_W();
	static void OnKeyDown_S();
	static void OnKeyUp_S();
	static void OnKeyDown_A();
	static void OnKeyUp_A();
	static void OnKeyDown_D();
	static void OnKeyUp_D();

	static char _movingFlag;
	static char _prevMovingFlag;
};

