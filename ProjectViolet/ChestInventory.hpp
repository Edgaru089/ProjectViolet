#pragma once

#include "InGameUI.hpp"


class ChestInventory :public InGameUI {
public:
	ChestInventory(Vector2i chestBlockCoord) :chestBlockCoord(chestBlockCoord) {}
	const string getUIId() override { return "chest_inventory"; }

	void runImGui() override;
	string windowTitle() override;
	bool showInventory() override { return true; }
private:
	Vector2i chestBlockCoord;
};

