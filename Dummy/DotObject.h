#pragma once


#include "GameObject.h"



class DotObject : public GameObject
{
public:
	DotObject();
	DotObject(ObjectID id, UserDBIndex ownerID);
	~DotObject();

	void MoveUp();
	void MoveDown();
	void MoveRight();
	void MoveLeft();

private:

};