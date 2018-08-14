#pragma once

#include "Block.hpp"
#include "TerrainManager.hpp"
#include "EntityManager.hpp"
#include "ItemEntity.hpp"
#include "ParticleSystem.hpp"


////////////////////////////////////////
Vector2i Block::getPosition() {
	return TerrainManager::convertChunkToWorldCoord(chunk, inChunkPos);
}


////////////////////////////////////////
DoubleRect Block::getHitbox() {
	Vector2i globalPos = TerrainManager::convertChunkToWorldCoord(chunk, inChunkPos);
	return DoubleRect(globalPos.x, globalPos.y, 1.0, 1.0);
}


////////////////////////////////////////
void Block::updateLogic() {
	_updateLogic();
}


////////////////////////////////////////
void Block::onDestroy(Entity * destoryer, bool drop) {

	if (drop) {
		// Emit particles
		particleSystem.emit(getHitbox(), getTextureInfo(), 8, 2.0, 10, seconds(2), seconds(3));

		// Tile Drop
		shared_ptr<ItemEntity> e = make_shared<ItemEntity>("block_" + getBlockId());
		for (auto& i : datasets.getDatasets()) {
			e->getDataset()[i.first] = i.second;
		}
		// Give a random velocity
		e->accelerateVector(1.0, 180 + rand() % 180);

		entityManager.insert(e, Vector2d(TerrainManager::convertChunkToWorldCoord(chunk, inChunkPos)) + Vector2d(0.5, 0.8));
	}
	_onDestroy(destoryer);
}
