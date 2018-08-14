#pragma once

#include "FallingSand.hpp"


////////////////////////////////////////
void FallingSand::_updateLogic() {
	if (terrainManager.getBlock(TerrainManager::convertWorldPositionToBlockCoord(getCenterPos())) == nullptr) {
		if (onGround) {
			Vector2i coord = TerrainManager::convertWorldPositionToBlockCoord(getCenterPos());
			terrainManager.placeBlock(coord, getData("block_id").getDataString());
			shared_ptr<Block> b = terrainManager.getBlock(coord);
			for (auto& i : datasets.getDatasets())
				if (i.first != "block_id")
					b->getDataset().getDatasets().insert(i);
			kill();
		}
	}
	else {
		if (onGround || isSame(vecY, 0)) {
			// Drop item
			// Borrow code from Block::onDestory()

			// Tile Drop
			shared_ptr<ItemEntity> e = make_shared<ItemEntity>("block_" + getData("block_id").getDataString());
			for (auto& i : datasets.getDatasets()) {
				e->getDataset().getDatasets().insert(i);
			}
			// Give a random velocity
			e->accelerateVector(1.0, 180 + rand() % 180);

			entityManager.insert(e, getPosition());

			kill();
		}
	}
}
