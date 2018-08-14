#pragma once

#include "GrenadeItem.hpp"
#include "EntityManager.hpp"
#include "GrenadeEntity.hpp"
#include "PlayerEntity.hpp"


////////////////////////////////////////
void GrenadeItem::_onRightReleased() {
	slotDataset["count"].getDataInt()--;
	if (slotDataset["count"].getDataInt() <= 0)
		slotDataset["item_name"].getDataString() = ""s;

	GrenadeEntity::throwGrenade();
}

