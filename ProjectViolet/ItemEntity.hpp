#pragma once

#include <functional>
#include <string>
#include <map>
#include "Main.hpp"
#include "Entity.hpp"


class ItemEntity :public Entity {
public:

	// name: block_<blockId> or item_<itemId>
	ItemEntity(string name = "stone") { setData("item_name", name); setData("picked_up", false); }

	virtual const string getItemName() { return getData("item_name").getDataString(); }
	TextureInfo getTextureInfo() override {
		return textureManager.getTextureInfo(getItemName());
	}

	const string getEntityId() override {
		return "item_entity";
	}

	Vector2d getSize() { return Vector2d(0.6, 0.6); }

	bool& pickedUp() { return getData("picked_up").getDataBool(); }
	int& count() { return getData("count").getDataInt(); }
	int& throwCooldownMilli() { return getData("throw_cooldown").getDataInt(); }

	void _updateLogic() override;

};


