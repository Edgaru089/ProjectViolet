#pragma once

#include "Mob.hpp"
#include "ParticleSystem.hpp"
#include "InventoryObject.hpp"


////////////////////////////////////////
void Mob::kill() {
	Vector2d t = getSize();
	double s = (t.x + t.y) / 2.0;
	particleSystem.emitSmoke(getHitbox(), 1.2, -.6, .2, 180, 360, 20 * t.x*t.y);
	particleSystem.emit(getHitbox(), getTextureInfo(), 5, 1.5f, t.x*t.y * 45, seconds(2), seconds(3), Vector2d(s, s)*.2);
	alive = false;
	try {
		dynamic_cast<InventoryObject&>(*this).dropAllItems(datasets, getCenterPos());
	}
	catch (bad_cast) {}
	_onKill();
}

