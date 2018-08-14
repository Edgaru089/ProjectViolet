#pragma once

#include "Main.hpp"
#include "Data.hpp"
#include "TextureManager.hpp"
#include "PlayerInventory.hpp"

class PlayerEntity;

class Item {
public:

	Item(Dataset& slot) :slotDataset(slot), focus(false) {}
	virtual ~Item() {}

	virtual const string getItemId() = 0;
	virtual TextureInfo getTextureInfo() { return textureManager.getTextureInfo("item_" + getItemId()); }
	virtual int getMaxItemsPerSlotCount() { return maxItemsPerSlot; }

	virtual void updateLogic() {}

	void setFocus(bool focus) { this->focus = focus; }
	bool isInHand() { return focus; }

	// Tool atturibs
	// Returns the damage caused when used as a weapon
	virtual const int getWeaponDamage() { return 1; }
	// Returns the factor multplied to the block-break time when used against stone blocks
	virtual const double getPickaxeTimeFactor() { return 1.0; }
	// Returns the factor multplied to the block-break time when used against wooden blocks
	virtual const double getAxeTimeFactor() { return 1.0; }
	// Returns the factor multplied to the block-break time when used against sand and dirt blocks
	virtual const double getShovelTimeFactor() { return 1.0; }


	// Returns false when nothing is done(and break block), true when the item is used
	virtual bool _onLeftPressed() { return false; }
	virtual void _onLeftReleased() {}
	// Returns false when nothing is done(and send right click signal to the block), true when the item is used
	virtual bool _onRightPressed() { return false; }
	virtual void _onRightReleased() {}

	virtual void _pushExtraImguiItemsToDashboard() {}

	Dataset& slotDataset;

private:

	bool focus;

};

