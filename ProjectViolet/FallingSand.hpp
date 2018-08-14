#pragma once

#include "Main.hpp"
#include "Entity.hpp"


class FallingSand :public Entity {
public:

	FallingSand() {}
	FallingSand(string id) { setData("block_id", id); }

	const string getEntityId() override { return "fallingsand"; }
	TextureInfo getTextureInfo() override { return textureManager.getTextureInfo("block_" + getData("block_id").getDataString()); }

	Vector2d getSize() override { return Vector2d(1 - 1e-7, 1 - 1e-7); }

	void _updateLogic() override;

private:

};

