#pragma once

#include "Item.hpp"

class BowItem :public Item {
public:
	BowItem(Dataset& data, string slotDataIdPrefix) :Item(data, slotDataIdPrefix) {}
	const string getItemId() override { return "bow"; }
	int getMaxItemsPerSlotCount() override { return 1; }
	TextureInfo getTextureInfo() override {
		if (!isLoading())
			return textureManager.getTextureInfo("item_bow");
		else
			return textureManager.getTextureInfo(StringParser::toStringF("item_bow_pulling_%d",
																		 min(3, 1 + loadedTimeMilli() / 400)));
	}

	void updateLogic() override;
	bool _onRightPressed() override;
	void _onRightReleased() override;

private:

	int& loadedTimeMilli() { return slotDataset[slotDataIdPrefix + "bow_loaded_time"].getDataInt(); }
	bool& isLoading() { return slotDataset[slotDataIdPrefix + "bow_loading"].getDataBool(); }

};
