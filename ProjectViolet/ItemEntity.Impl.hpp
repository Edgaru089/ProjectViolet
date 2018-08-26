
#include "ItemEntity.hpp"
#include "TerrainManager.hpp"


////////////////////////////////////////
void ItemEntity::_updateLogic() {
	if (throwCooldownMilli() > 0) {
		throwCooldownMilli() -= logicIO.deltaTime.asMilliseconds();
		if (throwCooldownMilli() < 0)
			throwCooldownMilli() = 0;
	}
}

