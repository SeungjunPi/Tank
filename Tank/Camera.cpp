
#include "Camera.h"
#include "Global.h"
#include "Player.h"
#include "Tank.h"

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

void GameCamera::Tick()
{
	assert(g_pPlayer != nullptr);

	Tank* pTank = g_pPlayer->GetTankPtr();
	if (pTank == nullptr) {
		return;
	}

	UpdateTransf(pTank->GetTransformPtr());
}
