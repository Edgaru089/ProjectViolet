#pragma once

#include "Item.hpp"

class GrenadeItem :public Item {
public:
	GrenadeItem(Dataset& data, string slotDataIdPrefix) :Item(data, slotDataIdPrefix) {}

	const string getItemId() override { return "grenade"; }
	int getMaxItemsPerSlotCount() override { return 8; }

	bool _onRightPressed() override { return true; };
	void _onRightReleased() override;

private:

};

