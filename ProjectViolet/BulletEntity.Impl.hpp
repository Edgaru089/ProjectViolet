#pragma once

#include "BulletEntity.hpp"
#include "EntityManager.hpp"
#include "Mob.hpp"


////////////////////////////////////////
void BulletEntity::shoot(double damage, double speed, double knockbackFactor, Vector2d position, double degree) {
	shared_ptr<BulletEntity> b = make_shared<BulletEntity>();
	b->damage = damage;
	b->knockback = knockbackFactor;
	b->accelerateVector(speed, degree);
	entityManager.insert(b, position + Vector2d(.0, b->getSize().y / 2.0));
}


////////////////////////////////////////
void BulletEntity::_onCollideEntity(shared_ptr<Entity> e) {
	if (!isAlive())
		return;
	if (e->getUuid() != localPlayer->getUuid()) {
		try {
			Mob& mob = dynamic_cast<Mob&>(*e);
			mob.harm(damage, getPosition(), knockback);
			kill();
		}
		catch (bad_cast) {}
	}
}


////////////////////////////////////////
void BulletEntity::_onCollision(shared_ptr<Block> b, CollisionBoxEdge) {
	if (!isAlive())
		return;
	if (b->isSolid())
		kill();
}

