#pragma once

#include "RpgEntity.hpp"
#include "IO.hpp"
#include "EntityManager.hpp"
#include "ParticleSystem.hpp"
#include "Mob.hpp"


////////////////////////////////////////
void RpgEntity::shoot(double force, Vector2d position, double degree, double speed) {
	shared_ptr<RpgEntity> e = make_shared<RpgEntity>();
	e->force = force;
	e->accelerateVector(speed, degree);
	entityManager.insert(e, position + Vector2d(.0, e->getSize().y / 2.0));
}


////////////////////////////////////////
void RpgEntity::_onCollision(shared_ptr<Block> block, CollisionBoxEdge) {
	if (!isAlive())
		return;
	if (!block->isSolid())
		return;
	entityManager.explode(getCenterPos(), force);
	kill();
}


////////////////////////////////////////
void RpgEntity::_onCollideEntity(shared_ptr<Entity> e) {
	if (!isAlive())
		return;
	if (e->getUuid() == localPlayer->getUuid())
		return;
	try {
		Mob& mob = dynamic_cast<Mob&>(*e);
		entityManager.explode(getCenterPos(), force);
		kill();
	}
	catch (bad_cast) {}
}


////////////////////////////////////////
void RpgEntity::_updateLogic() {
	// Particles when RPG is flying
	if (particleClock.getElapsedTime() >= milliseconds(15)) {
		particleClock.restart();
		particleSystem.emitSmoke(getCenterPos(), 5.0, -0.1, 0.2, getRotation() + 180.0 - 8.0, getRotation() + 180.0 + 8.0, 1);
	}
}


