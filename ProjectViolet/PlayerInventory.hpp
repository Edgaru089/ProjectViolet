#pragma once

#include "Main.hpp"
#include "Data.hpp"


const int maxItemsPerSlot = 64;

class PlayerInventory {
public:

	PlayerInventory();

	void updateLogic();

	void runImGui();

	int getCursorId() { return cursorId; }
	void addCursorId() { cursorId++; cursorId %= 9; }
	void minusCursorId() { cursorId--; if (cursorId < 0) cursorId += 9; }

	// Integer - count
	// String - item_name
	// Uses localPlayer->dataset
	int cursorId;

};


PlayerInventory playerInventory;

