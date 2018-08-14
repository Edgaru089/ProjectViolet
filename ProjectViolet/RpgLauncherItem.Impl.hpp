#pragma once

#include "RpgLauncherItem.hpp"
#include "RpgEntity.hpp"


////////////////////////////////////////
void RpgLauncherItem::_onShoot() {
	RpgEntity::shoot(RpgExplodeForce, localPlayer->getEyePosition(),
					 gameIO.degreeAngle + (rand01() - 0.5)*bulletUnaccuracyDegree());
}

