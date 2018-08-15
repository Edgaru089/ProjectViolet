#pragma once

#include "GunItem.hpp"

class RpgLauncherItem :public GunItem {
public:
	RpgLauncherItem(Dataset& data, string slotDataIdPrefix) :GunItem(data, slotDataIdPrefix) {}

	const string getItemId() override { return "rpg_launcher"; }
	TextureInfo getTextureInfo() override {
		if (roundsLeft() > 0)
			return textureManager.getTextureInfo("item_rpg_launcher");
		else
			return textureManager.getTextureInfo("item_rpg_launcher_empty");
	}

	void _onShoot() override;

	Time shootInterval() override { return milliseconds(1); }  // Does not matter; there's only one round after all
	double bulletDamage() override { return .0; }              // Also doesn't matter
	double bulletUnaccuracyDegree() override { return 6.0; }
	Time reloadTime() override { return seconds(4.0f); }
	int roundsPerMagazine() override { return 1; }
	string magazineItemName() override { return "item_rpg_ammo"; }
};

class RpgAmmoItem :public Item {
public:
	RpgAmmoItem(Dataset& data, string slotDataIdPrefix) :Item(data, slotDataIdPrefix) {}
	const string getItemId() override { return "rpg_ammo"; }
	int getMaxItemsPerSlotCount() override { return 16; }
};
