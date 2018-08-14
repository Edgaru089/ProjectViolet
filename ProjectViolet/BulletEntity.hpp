#pragma once

#include "Entity.hpp"
#include "PlayerEntity.hpp"
#include "Mob.hpp"

class BulletEntity :public Entity {
public:

	static void shoot(double damage = 5,
					  double speed = 25.0,
					  double knockbackFactor = 1.0,
					  Vector2d position = localPlayer->getEyePosition(),
					  double degree = gameIO.degreeAngle);

public:

	const double getGravityAclc() override { return .0; }
	const string getEntityId() override { return "bullet"; }

	const bool requestEntityCollisionCallback() override { return true; }
	virtual void _onCollideEntity(shared_ptr<Entity> e) override;
	virtual void _onCollision(shared_ptr<Block> block, CollisionBoxEdge) override;

protected:

	double damage;
	double knockback;

};
