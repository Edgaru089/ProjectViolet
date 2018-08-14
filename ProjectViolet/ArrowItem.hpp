#pragma once

#include "Item.hpp"

class ArrowItem :public Item {
public:
	ArrowItem(Dataset& data) :Item(data) {}
	const string getItemId() override { return "arrow"; }
};
