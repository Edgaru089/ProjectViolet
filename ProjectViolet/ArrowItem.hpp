#pragma once

#include "Item.hpp"

class ArrowItem :public Item {
public:
	ArrowItem(Dataset& data, string slotDataIdPrefix) :Item(data, slotDataIdPrefix) {}
	const string getItemId() override { return "arrow"; }
};
