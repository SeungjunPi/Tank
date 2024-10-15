#pragma once


struct KeyboardInputs
{
	bool up;
	bool down;
	bool left;
	bool right;
	bool spawn;
	bool despawn;
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

	static EKeyboardMovementStatus GetAndUpdateKeyboardMovingStatus();

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

