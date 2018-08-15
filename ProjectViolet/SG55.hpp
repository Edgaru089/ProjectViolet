#pragma once

#include "GunItem.hpp"

class SG55 :public GunItem {
public:
	SG55(Dataset& data, string slotDataIdPrefix) :GunItem(data, slotDataIdPrefix) {}

	const string getItemId() override { return "sg55"; }

	Time shootInterval() override { return seconds(0.3f); }
	double bulletDamage() override { return 4.0; } // 6 rounds were fired at the same time, however.
	double bulletUnaccuracyDegree() override { return 12.0; }
	double bulletSpeed() override { return 35.0; }
	Time reloadTime() override { return seconds(1.5f); }
	int roundsPerMagazine() override { return 12; }
	string magazineItemName() override { return "item_sg55_ammo"; }

	void _onShoot() override;
};

class SG55Ammo :public Item {
public:
	SG55Ammo(Dataset& d, string slotDataIdPrefix) :Item(d, slotDataIdPrefix) {}
	const string getItemId() override { return "sg55_ammo"; }
	int getMaxItemsPerSlotCount() override { return 8; }
};
