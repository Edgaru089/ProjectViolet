#pragma once

#include "GunItem.hpp"

class MRM36 :public GunItem {
public:
	MRM36(Dataset& data) :GunItem(data) {}

	const string getItemId() override { return "mrm36"; }

	Time shootInterval() override { return seconds(1.0f); }
	double bulletDamage() override { return 20.0; }
	double bulletUnaccuracyDegree() override { return 1.0; }
	double bulletSpeed() override { return 20.0; }
	Time reloadTime() override { return seconds(3.0f); }
	int roundsPerMagazine() override { return 9; }
	string magazineItemName() override { return "item_mrm36_ammo"; }

	void _onShoot() override;
};

class MRM36Ammo :public Item {
public:
	MRM36Ammo(Dataset& d) :Item(d) {}
	const string getItemId() override { return "mrm36_ammo"; }
	int getMaxItemsPerSlotCount() override { return 4; }
};
