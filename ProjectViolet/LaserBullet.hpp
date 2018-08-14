#pragma once

#include "Main.hpp"
#include "BulletEntity.hpp"

class LaserBullet :public BulletEntity {
public:

	static void shoot(double damage = 5,
					  double speed = 25.0,
					  double knockbackFactor = 1.0,
					  Vector2d position = localPlayer->getEyePosition(),
					  double degree = gameIO.degreeAngle);

public:

	const string getEntityId() override { return "laser_bullet"; }

	void _updateLogic() override;
	void _onCollideEntity(shared_ptr<Entity> e) override;

private:
	set<Uuid> hitMobs;
	Clock particleClock;
};
