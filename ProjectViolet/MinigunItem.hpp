#pragma once

#include "Item.hpp"
#include "GunItem.hpp"

class MinigunItem :public GunItem {
public:
	MinigunItem(Dataset& data, string slotDataIdPrefix) :GunItem(data, slotDataIdPrefix) {}

	const string getItemId() override { return "minigun"; }

	Time shootInterval() override { return milliseconds(100); } // 600 RPM
	double bulletDamage() override { return 2.0; }
	double bulletUnaccuracyDegree() override { return 4.0; }
	Time reloadTime() override { return seconds(3.5f); }
	int roundsPerMagazine() override { return 150; }
	string magazineItemName() override { return "item_minigun_ammo"; }

};

class MinigunAmmoItem :public Item {
public:
	MinigunAmmoItem(Dataset& d,string slotDataIdPrefix) :Item(d, slotDataIdPrefix) {}
	const string getItemId() override { return "minigun_ammo"; }
	int getMaxItemsPerSlotCount() override { return 8; }
};

