#pragma once

#include "VTX65.hpp"
#include "LaserBullet.hpp"


////////////////////////////////////////
void VTX65::_onShoot() {
	LaserBullet::shoot(bulletDamage(), bulletSpeed(), bulletKnockbackFactor(), localPlayer->getEyePosition(),
					   gameIO.degreeAngle + (rand01() - 0.5)*bulletUnaccuracyDegree());
}

