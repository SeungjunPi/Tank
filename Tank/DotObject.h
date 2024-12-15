#pragma once


#include "GameObject.h"



class DotObject : public GameObject
{
public:
	DotObject();
	DotObject(ObjectID id);
	~DotObject();

	void MoveUp();
	void MoveDown();
	void MoveRight();
	void MoveLeft();

private:

};