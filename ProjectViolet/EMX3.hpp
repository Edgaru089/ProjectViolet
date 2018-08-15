#pragma once

#include "GunItem.hpp"

class EMX3 :public GunItem {
public:
	EMX3(Dataset& data, string slotDataIdPrefix) :GunItem(data, slotDataIdPrefix) {}

	const string getItemId() override { return "emx3"; }

	Time shootInterval() override { return milliseconds(50); } // 1200 RPM
	double bulletDamage() override { return 2.0; }
	double bulletUnaccuracyDegree() override { return 3.5; }
	double bulletKnockbackFactor() override { return 0.6; }
	Time reloadTime() override { return seconds(3.5f); }
	int roundsPerMagazine() override { return 350; }
	string magazineItemName() override { return "item_emx3_ammo"; }

};

class EMX3Ammo :public Item {
public:
	EMX3Ammo(Dataset& d, string slotDataIdPrefix) :Item(d, slotDataIdPrefix) {}
	const string getItemId() override { return "emx3_ammo"; }
	int getMaxItemsPerSlotCount() override { return 6; }
};

