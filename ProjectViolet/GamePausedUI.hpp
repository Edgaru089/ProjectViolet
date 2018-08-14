#pragma once

#include "InGameUI.hpp"


class GamePausedUI :public InGameUI {
public:
	const string getUIId() override { return "game_paused_ui"; }
	const bool isBlocking() override { return true; }

	void runImGui() override;

	string windowTitle() override;
	bool showInventory() { return false; };
};

