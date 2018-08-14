#pragma once

#include "MRM36.hpp"
#include "IO.hpp"
#include "RpgEntity.hpp"


////////////////////////////////////////
void MRM36::_onShoot() {
	RpgEntity::shoot(bulletDamage(),
					 localPlayer->getEyePosition(),
					 gameIO.degreeAngle + (rand01() - .5)*bulletUnaccuracyDegree(),
					 24.0);
}

