#pragma once

#include "LaserBullet.hpp"
#include "ParticleSystem.hpp"


////////////////////////////////////////
void LaserBullet::shoot(double damage, double speed, double knockbackFactor, Vector2d position, double degree) {
	shared_ptr<LaserBullet> b = make_shared<LaserBullet>();
	b->damage = damage;
	b->knockback = knockbackFactor;
	b->accelerateVector(speed, degree);
	entityManager.insert(b, position + Vector2d(.0, b->getSize().y / 2.0));
}


////////////////////////////////////////
void LaserBullet::_updateLogic() {
	//if (particleClock.getElapsedTime() > milliseconds(30)) {
	//	particleClock.restart();
	particleSystem.emit(getCenterPos(), textureManager.getTextureInfo("particle_laser_glow"), 1.0, .0, 1, seconds(.2f), seconds(.2f), Vector2d(.25, .25), .0);
//}
}


////////////////////////////////////////
void LaserBullet::_onCollideEntity(shared_ptr<Entity> e) {
	// Borrow modified code from BulletEntity::_onCollideEntity()
	if (!isAlive())
		return;
	if (e->getUuid() != localPlayer->getUuid() && hitMobs.find(e->getUuid()) == hitMobs.end()) {
		try {
			Mob& mob = dynamic_cast<Mob&>(*e);
			mob.harm(damage, getPosition(), knockback);
			hitMobs.insert(mob.getUuid());
		}
		catch (bad_cast) {}
	}
}

