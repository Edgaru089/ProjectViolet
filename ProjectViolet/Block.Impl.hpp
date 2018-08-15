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

