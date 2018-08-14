#pragma once

#include "GrenadeEntity.hpp"
#include "EntityManager.hpp"
#include "PlayerEntity.hpp"


////////////////////////////////////////
void GrenadeEntity::throwGrenade(Time triggerTime, double degree, Vector2d pos) {
	shared_ptr<GrenadeEntity> e = make_shared<GrenadeEntity>();
	e->accelerateVector(10.0, gameIO.degreeAngle);
	e->liveClock.restart();
	e->triggerTime = triggerTime;
	entityManager.insert(e, localPlayer->getEyePosition() + Vector2d(.0, e->getSize().y / 2.0));
}


////////////////////////////////////////
void GrenadeEntity::_updateLogic() {
	if (wantSetSpeed) {
		vecX = setSpeed.x;
		vecY = setSpeed.y;
		wantSetSpeed = false;
	}
	if (liveClock.getElapsedTime() > triggerTime) {
		entityManager.explode(getCenterPos(), grenadeDamage);
		kill();
	}
}


////////////////////////////////////////
void GrenadeEntity::_onCollision(shared_ptr<Block> b, CollisionBoxEdge whichEntityEdge) {
	if (!isAlive())
		return;
	if (b->isSolid()) {
		// Rebound on contact
		wantSetSpeed = true;
		setSpeed = Vector2d(vecX, vecY)*0.45;
		if (whichEntityEdge == Left || whichEntityEdge == Right)
			setSpeed.x = -setSpeed.x;
		if (whichEntityEdge == Top || whichEntityEdge == Bottom)
			setSpeed.y = -setSpeed.y;
	}
}

