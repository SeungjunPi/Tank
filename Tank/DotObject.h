#pragma once


#include "GameObject.h"



class DotObject : public GameObject
{
public:
	DotObject();
	DotObject(UINT32 id);
	~DotObject();

	void MoveUp();
	void MoveDown();
	void MoveRight();
	void MoveLeft();

private:

};