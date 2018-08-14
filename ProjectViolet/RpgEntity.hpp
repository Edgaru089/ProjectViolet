#pragma once

#include "Entity.hpp"
#include "PlayerEntity.hpp"

const double RpgExplodeForce = 25.0;

class RpgEntity :public Entity {
public:

	static void shoot(double force = RpgExplodeForce,
					  Vector2d position = localPlayer->getEyePosition(),
					  double degree = gameIO.degreeAngle,
					  double speed = 16.0);

public:

	RpgEntity() {}

	const string getEntityId() override { return "rpg"; }

	const bool requestEntityCollisionCallback() override { return true; }
	void _onCollideEntity(shared_ptr<Entity> e) override;

	const double getGravityAclc() override { return 1.5; }

	Vector2d getSize() override { return Vector2d(0.2, 0.2); }

	void _updateLogic() override;
	void _onCollision(shared_ptr<Block> block, CollisionBoxEdge whichEntityEdge) override;

private:

	Clock particleClock;

	double force;

};

