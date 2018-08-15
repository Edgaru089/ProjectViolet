#pragma once

#include "Item.hpp"


class GunItem :public Item {
public:
	GunItem(Dataset& data, string slotDataIdPrefix) :Item(data, slotDataIdPrefix) {}

	virtual const string getItemId() = 0;

	virtual Time shootInterval() = 0;
	virtual double bulletDamage() = 0;
	virtual double bulletSpeed() { return 25.0; }
	virtual double bulletUnaccuracyDegree() { return 2.0; }
	virtual double bulletKnockbackFactor() { return 1.0; }
	virtual Time reloadTime() = 0;
	virtual int roundsPerMagazine() = 0;
	virtual string magazineItemName() = 0; // "item_xxx"

	virtual void _onShoot();

	int getMaxItemsPerSlotCount() override { return 1; }

	void updateLogic() override;

	bool _onRightPressed() override;
	void _onRightReleased() override;

	void _pushExtraImguiItemsToDashboard() override;

protected:

	int& shootCooldownMill() { return slotDataset[slotDataIdPrefix + "shoot_cooldown"].getDataInt(); }
	bool& isShooting() { return slotDataset[slotDataIdPrefix + "is_shooting"].getDataBool(); };
	int& roundsLeft() { return slotDataset[slotDataIdPrefix + "rounds_left"].getDataInt(); }
	int& reloadedTimeMill() { return slotDataset[slotDataIdPrefix + "reloaded_time"].getDataInt(); }
	bool& isReloading() { return slotDataset[slotDataIdPrefix + "is_reloading"].getDataBool(); }

};

