#pragma once

#include "GunItem.hpp"

class VTX65 :public GunItem {
public:
	VTX65(Dataset& data) :GunItem(data) {}

	const string getItemId() override { return "vtx65"; }

	Time shootInterval() override { return seconds(0.5); }
	double bulletDamage() override { return 20.0; }
	double bulletUnaccuracyDegree() override { return 0.5; }
	double bulletSpeed() override { return 20.0; }
	Time reloadTime() override { return seconds(2.5f); }
	int roundsPerMagazine() override { return 12; }
	string magazineItemName() override { return "item_vtx65_ammo"; }

	void _onShoot() override;
};

class VTX65Ammo :public Item {
public:
	VTX65Ammo(Dataset& d) :Item(d) {}
	const string getItemId() override { return "vtx65_ammo"; }
	int getMaxItemsPerSlotCount() override { return 6; }
};

