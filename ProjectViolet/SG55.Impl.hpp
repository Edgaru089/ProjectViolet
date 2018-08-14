#pragma once

#include "Main.hpp"
#include "SG55.hpp"
#include "BulletEntity.hpp"


////////////////////////////////////////
void SG55::_onShoot() {
	for (int i = 1; i <= 6; i++) {
		BulletEntity::shoot(bulletDamage(),
							bulletSpeed()*(1.0 - rand01() / 16.0),
							0.3,
							localPlayer->getEyePosition(),
							gameIO.degreeAngle + bulletUnaccuracyDegree()*(rand01() - 0.5));
	}
}

