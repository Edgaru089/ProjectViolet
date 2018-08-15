#pragma once

#include "Main.hpp"
#include "Data.hpp"
#include "TextureManager.hpp"
#include "PlayerInventory.hpp"

class PlayerEntity;

class Item {
public:

	Item(Dataset& slot, string slotDataIdPrefix) :slotDataset(slot), slotDataIdPrefix(slotDataIdPrefix), focus(false) {}
	virtual ~Item() {}

	virtual const string getItemId() = 0;
	virtual TextureInfo getTextureInfo() { return textureManager.getTextureInfo("item_" + getItemId()); }
	virtual int getMaxItemsPerSlotCount() { return maxItemsPerSlot; }

	virtual void updateLogic() {}

	void setFocus(bool focus) { this->focus = focus; }
	bool isInHand() { return focus; }

	// Returns false when nothing is done, true when the item is used
	virtual bool _onLeftPressed() { return false; }
	virtual void _onLeftReleased() {}
	// Returns false when nothing is done(and send right click signal to the block), true when the item is used
	virtual bool _onRightPressed() { return false; }
	virtual void _onRightReleased() {}

	virtual void _pushExtraImguiItemsToDashboard() {}

	Dataset& slotDataset;
	const string slotDataIdPrefix;

protected:

	bool focus;

};

