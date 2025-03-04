#include "Tank.h"
#include "Global.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "PlayerManager.h"
#include "ObjectManager.h"
#include "GameStruct.h"
#include "ServerPacketHandler.h"




Tank::Tank(ObjectID id, UserDBIndex ownerId)
	: GameObject(id, ownerId, true)
{
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

	Vector3 v = { 0.0f, -1.0f, 0.0f };
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

void Tank::Tick(ULONGLONG tickDiff)
{
	GameObject::Tick(tickDiff);
	if (!IsAlive()) {
		if (g_currentGameTick - _hitTick > TICK_TANK_RESPAWN_INTERVAL) {
			Respawn();
			ServerPacketHandler::BroadcastRespawnTank(_id);
		}
	}
	return;
}

BOOL Tank::IsDestroyed(ULONGLONG currentTick) const
{
	return !_isActivatable && (!_isAlive || _hitTick != 0);
}

void Tank::OnHitWith(ULONGLONG currentTick, GameObject* other)
{
	ObjectID otherID = other->GetID();
	if (_ownerIndex == other->GetOwnerId()) {
		return;
	}

	if (otherID.type == GAME_OBJECT_TYPE_PROJECTILE) {
		if (_hp > 0) {
			if (--_hp == 0) {
				_pCollider->Deactivate();
				ResetDynamicPhysicalComponent();
				_hitTick = currentTick;
				_isAlive = false;
				
				g_playerManager.IncreaseDeathCount(_ownerIndex);
				g_playerManager.IncreaseKillCount(other->GetOwnerId());
			}
			g_playerManager.IncreaseHitCount(other->GetOwnerId());
			ServerPacketHandler::BroadcastHit(_id, other->GetID());
		}
	}
}

void Tank::OnUpdateTransform()
{
	// printf("P[%f, %f, %f], R[%f, %f, %f, %f]\n", _physicalComponent.transform.Position.x, _physicalComponent.transform.Position.y, _physicalComponent.transform.Position.z, _physicalComponent.transform.Rotation.w, _physicalComponent.transform.Rotation.x, _physicalComponent.transform.Rotation.y, _physicalComponent.transform.Rotation.z);
}

void Tank::Respawn()
{
	GameObject::Respawn();
	_hitTick = 0;
	_pCollider->Activate();
	ResetHP();
}

BOOL Tank::IsTransformCloseEnough(const Transform* other)
{
	const float TOLERANCE_POSITION_SQUARE = 0.5f; // x, y, z의 차이가 각 1.0 미만인 경우를 간단하게 판단
	const float TOLERANCE_ROTATION = 0.13f; // 15도 미만

	float posDistanceSq = Vector3::DistanceSquared(_physicalComponent.transform.Position, other->Position);
	float rotDisctance = Quaternion::AngularDistance(_physicalComponent.transform.Rotation, other->Rotation);

	if (posDistanceSq < TOLERANCE_POSITION_SQUARE && rotDisctance < TOLERANCE_ROTATION) {
		// printf("Accept Client's Transform, Diff: [%f, %f]\n", posDistanceSq, rotDisctance);
		return true;
	}
	else {
		// printf("Accept Server's Transform, Diff: [%f, %f]\n", posDistanceSq, rotDisctance);
		return false;
	}
}

BOOL Tank::TryFireMachineGun(ULONGLONG currentTick)
{
	/*if (currentTick < _lastMachineGunFiringTick + TANK_MACHINE_GUN_DELAY) {
		return false;
	}

	Transform projectileTransform;
	Vector3 direction;
	GetTurretInfo(&projectileTransform.Position, &direction);
	projectileTransform.Position = projectileTransform.Position + direction;

	GamePacket::SendFireMachineGun(&projectileTransform);*/
	return true;
}




