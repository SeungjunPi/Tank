
#include "Camera.h"

GameCamera::GameCamera()
{
	_transf = { .0f, .0f, .0f, 1.0f , .0f , .0f , .0f };
	
}

Vector3 GameCamera::GetPosition() const
{
	Vector3 pos;
	pos.x = _transf.Position.x;
	pos.y = _transf.Position.y;
	pos.z = _transf.Position.z;

	return pos;
}

void GameCamera::UpdateTransf(const Transform* to)
{
	memcpy(&_transf, to, sizeof(Transform));
}
