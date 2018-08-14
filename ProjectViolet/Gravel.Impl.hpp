#pragma once

#include "Gravel.hpp"
#include "TerrainManager.hpp"
#include "EntityManager.hpp"
#include "FallingSand.hpp"


////////////////////////////////////////
void Gravel::_updateLogic() {
	if ((terrainManager.getBlock(TerrainManager::convertChunkToWorldCoord(chunk, inChunkPos) + Vector2i(0, 1)) == nullptr ||
		 !terrainManager.getBlock(TerrainManager::convertChunkToWorldCoord(chunk, inChunkPos) + Vector2i(0, 1))->isSolid()) &&
		terrainManager.getChunk(TerrainManager::convertWorldCoordToChunkId(
			TerrainManager::convertChunkToWorldCoord(chunk, inChunkPos) + Vector2i(0, 1))) != nullptr) {
		entityManager.insert(make_shared<FallingSand>("gravel"s),
							 Vector2d(TerrainManager::convertChunkToWorldCoord(chunk, inChunkPos)) + Vector2d(0.5, 1));
		terrainManager.setBlock(TerrainManager::convertChunkToWorldCoord(chunk, inChunkPos), "");
	}
}

