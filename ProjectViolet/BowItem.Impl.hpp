#pragma once

#include "BowItem.hpp"


////////////////////////////////////////
void BowItem::updateLogic() {
	loadedTimeMilli() += logicIO.deltaTime.asMilliseconds();
}


////////////////////////////////////////
bool BowItem::_onRightPressed() {
	// TODO Check inventory and arrows before shooting
	bool ok = false;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 9; j++) {
			auto& slot = localPlayer->getDataset();
			if (slot[to_string(i) + to_string(j) + "item_name"].getDataString() == "item_arrow") {
				slot[to_string(i) + to_string(j) + "count"].getDataInt()--;
				if (slot[to_string(i) + to_string(j) + "count"].getDataInt() <= 0)
					slot[to_string(i) + to_string(j) + "item_name"].getDataString() = "";
				ok = true;
				break;
			}
		}
	if (ok) {
		loadedTimeMilli() = 0;
	}
	return true;
}


////////////////////////////////////////
void BowItem::_onRightReleased() {
	if (slotDataset[slotDataIdPrefix + "bow_start_time"].getDataInt() != 0) {
		double stage = min(1.0, 0.2 +
			(double)(loadedTimeMilli()) / 1200.0 * 0.8);
		slotDataset[slotDataIdPrefix + "bow_start_time"].setData(0);
		// TODO Force and damage change
		ArrowEntity::shoot(stage*maxArrowDamage);
	}
}
