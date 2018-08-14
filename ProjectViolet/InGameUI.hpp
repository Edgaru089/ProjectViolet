#pragma once

#include "Main.hpp"


class InGameUI {
public:
	virtual const string getUIId() = 0;
	virtual const bool isBlocking() { return false; }

	virtual void runImGui() = 0;

	virtual string windowTitle() = 0;
	virtual bool showInventory() = 0;

	virtual void _onOpen() {}
	virtual void _onClose() {}
};


class InGameUIManager :public Lockable {
public:

	~InGameUIManager();

	void updateLogic();


	void runImGui();

	bool isUIOpen() { return curUI != nullptr; }

public:

	void changeUI(shared_ptr<InGameUI> gui) {
		lock();
		wantChange = true;
		wantChangeUI = gui;
		unlock();
	}

	shared_ptr<InGameUI> getCurrentUI() { return curUI; }

private:

	void _runInventoryUI();

	atomic_bool wantChange;
	shared_ptr<InGameUI> curUI, wantChangeUI;

};

InGameUIManager uiManager;


class PlayerInventoryUI :public InGameUI {
public:
	const string getUIId() override { return "player_inventory_ui"; }

	static void ImGuiInventorySlot(Dataset& slotData, int pushId = -1);

	virtual void runImGui() override;
	virtual string windowTitle() override { return "Player Inventory"; }
	virtual bool showInventory() override { return true; }
};
