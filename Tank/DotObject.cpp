#include "DotObject.h"

static Vertex DOT_VERTEX[1] = { 0, 0, 0, '*' };
static const Model MODEL_DOT = { DOT_VERTEX, 1 };

DotObject::DotObject()
{
	_model = MODEL_DOT;
}

DotObject::DotObject(ObjectID id, UserDBIndex ownerID)
	: GameObject(id, ownerID)
{
	_model = MODEL_DOT;
}

DotObject::~DotObject()
{
}

void DotObject::MoveUp()
{
	_transform.Position.y -= 1;

	_dirty = true;
}

void DotObject::MoveDown()
{
	_transform.Position.y += 1;
	_dirty = true;
}

void DotObject::MoveRight()
{
	_transform.Position.x += 1;
	_dirty = true;
}

void DotObject::MoveLeft()
{
	_transform.Position.x -= 1;
	_dirty = true;
}
