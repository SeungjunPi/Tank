#include "Tank.h"
#include "StaticData.h"
#include "Global.h"
#include "NetworkProtocol.h"
#include "Collider.h"
#include "IStableFlow.h"
#include "AllocObjectManager.h"
#include "Camera.h"
#include "Player.h"

Tank::Tank(ObjectID id, UserDBIndex ownerID)
	: GameObject(id, ownerID, true)
{	
	_model = g_pTankModel;
	_physicalComponent.mass = TANK_COLLIDER_MASS;
	_physicalComponent.radius = TANK_COLLIDER_RADIUS;
	_physicalComponent.colliderType = COLLIDER_TYPE_SPHERE;

	ResetHP();
}

Tank::~Tank()
{
}

void Tank::GetTurretInfo(Transform* out_position, Vector3* out_direction) const
{
	Vector3 position = _physicalComponent.transform.Position;
	Quaternion rotation = _physicalComponent.transform.Rotation;

	Vector3 v = _model.vertices[0].v;
	v = Vector3::Rotate(v, rotation);
	v.x += position.x;
	v.y += position.y;
	v.z += position.z;
	out_position->Position = v;
	out_position->Rotation = _physicalComponent.transform.Rotation;

	Vector3 direction = FORWARD_DIRECTION;
	direction = direction * (_physicalComponent.radius + PROJECTILE_COLLIDER_RADIUS) * 1.03125f;

	const Vector3 forwardDirection = Vector3::Rotate(direction, rotation);

	memcpy(out_direction, &forwardDirection, sizeof(Vector3));
}

void Tank::ResetHP()
{
	_hp = 5;
}

void Tank::UpdatePlayerInputStateFromServer(PlayerInputState inputState)
{
	if (_ownerID == g_pPlayer->GetUserID()) {
		return;
	}
	AdvancePlayerInput(inputState);
}

void Tank::AdvancePlayerInput(PlayerInputState inputState)
{
	if ((inputState & FLAG_PLAYER_INPUT_FORWARD) && !(inputState & FLAG_PLAYER_INPUT_BACKWARD)) {
		_physicalComponent.velocity = Vector3::Rotate(FORWARD_DIRECTION, _physicalComponent.transform.Rotation) * TANK_TRANSLATION_SPEED;
	}
	else if ((inputState & FLAG_PLAYER_INPUT_BACKWARD) && !(inputState & FLAG_PLAYER_INPUT_FORWARD)) {
		_physicalComponent.velocity = Vector3::Rotate(BACKWARD_DIRECTION, _physicalComponent.transform.Rotation) * TANK_TRANSLATION_SPEED;
	}
	else {
		_physicalComponent.velocity = Vector3();
	}

	if (inputState & FLAG_PLAYER_INPUT_ROTATE_LEFT) {
		if (!(inputState & FLAG_PLAYER_INPUT_ROTATE_RIGHT)) {
			_physicalComponent.angularVelocity = { 0, 0, -TANK_ROTATION_SPEED };
		}
		else {
			_physicalComponent.angularVelocity = Vector3();
		}
	}
	else if (inputState & FLAG_PLAYER_INPUT_ROTATE_RIGHT) {
		_physicalComponent.angularVelocity = { 0, 0, TANK_ROTATION_SPEED };
	}
	else {
		_physicalComponent.angularVelocity = Vector3();
	}
}

BOOL Tank::IsDestroyed(ULONGLONG currentTick) const
{
	return !_isActivatable && (!_isAlive || _hitTick != 0);
}

void Tank::Tick(ULONGLONG tickDiff)
{
	GameObject::Tick(tickDiff);
	//printf("[%f, %f, %f, %f]\n", _physicalComponent.transform.Rotation.w, _physicalComponent.transform.Rotation.x, _physicalComponent.transform.Rotation.y, _physicalComponent.transform.Rotation.z);
	
	return;
}

void Tank::OnHitWith(ULONGLONG currentTick, GameObject* other)
{
	
}

void Tank::OnUpdateTransform()
{
	
}

void Tank::OnRespawn()
{
	GameObject::OnRespawn();
	_hitTick = 0;
	_pCollider->Activate();
	ResetHP();
}

void Tank::OnHitServer(ULONGLONG currentTick, GameObject* other)
{
	ObjectID otherID = other->GetID();
	if (otherID.type == GAME_OBJECT_TYPE_PROJECTILE) {
		if (_hp > 0) {
			if (--_hp == 0) {
				_pCollider->Deactivate();
				_hitTick = currentTick;
				_isAlive = false;
				ResetDynamicPhysicalComponent();

				if (_id.equals(g_pPlayer->GetTankID())) {
					g_pPlayer->IncreaseDeath();
				}

				if (other->GetOwnerID() == g_pPlayer->GetUserID()) {
					g_pPlayer->IncreaseKill();
				}
			}

			if (other->GetOwnerID() == g_pPlayer->GetUserID()) {
				g_pPlayer->IncreaseHit();
			}
		}
	}
}

BOOL Tank::TryFireMachineGun(ULONGLONG currentTick)
{
	if (currentTick < _lastMachineGunFiringTick + TANK_MACHINE_GUN_DELAY) {
		return false;
	}

	_lastMachineGunFiringTick = g_currentGameTick;

	Transform projectileTransform;
	Vector3 direction;
	GetTurretInfo(&projectileTransform, &direction);

	// 자신이 맞지 않기 위해 여유분을 1.0625만큼 줌
	projectileTransform.Position = projectileTransform.Position + direction * 1.0625f; 
	
	PacketHandler::s_CSSendFireMachineGun(&projectileTransform, g_pPlayer->GetSessionID());
	return true;
}


