#pragma once

#include "TerrainGenerator.hpp"
#include "TerrainManager.hpp"
#include "Stone.hpp"


////////////////////////////////////////
void TerrainGenerator::setup(Vector2u chunkCount, int spawnCount) {
	terrainManager.clearChunks();

	this->chunkCount = chunkCount;
	this->spawnCount = spawnCount;
	terrainManager.setChunkCount(Vector2i(chunkCount));

	width = chunkCount.x*chunkSize;
	height = chunkCount.y*chunkSize;

	// Start out with filled stones
	for (int i = 0; i < chunkCount.x; i++) {
		for (int j = 0; j < chunkCount.y; j++) {
			auto c = terrainManager.loadEmptyChunk(Vector2i(i, j));
			for (int i = 0; i < chunkSize; i++)
				for (int j = 0; j < chunkSize; j++)
					c->setBlock(Vector2i(i, j), make_shared<Stone>());
		}
	}

	// Generate spawns
	spawnPoints.reserve(spawnCount);
	for (int i = 1; i <= spawnCount; i++) {
		spawnPoints.push_back(Vector2i((width - 6)*rand01() + 3, (height - 6)*rand01() + 3));
	}

	// Generate Buildings
	/* Spawn
	 **********Ls*****
	 **LLLLLLLLLss****
	 ****Lss...Lss****
	 **..Lss..|.ss..**
	 *...Lsssx.Csss..*
	 *ppppspppppppppp*
	 *: stone; x: spawn; s: gravel; |: torch; L: log;
	 p: planks; C: chest; .: air.
	 */
	for (int l = 0; l < spawnCount; l++) {
		int x = spawnPoints[l].x, y = spawnPoints[l].y;

		// Clear out a 9x3 space
		for (int i = -4; i <= 4; i++)
			for (int j = -2; j <= 0; j++)
				terrainManager.setBlock(Vector2i(x + i, y + j), "NULL");
		// Clear out another 13x2 space
		for (int i = -6; i <= 6; i++)
			for (int j = -1; j <= 0; j++)
				terrainManager.setBlock(Vector2i(x + i, y + j), "NULL");

		terrainManager.setBlock(Vector2i(x + 7, y), "NULL");
		terrainManager.setBlock(Vector2i(x - 7, y), "NULL");


		// Logs
		terrainManager.setBlock(Vector2i(x - 4, y), "log");
		terrainManager.setBlock(Vector2i(x - 4, y - 1), "log");
		terrainManager.setBlock(Vector2i(x - 4, y - 2), "log");
		terrainManager.setBlock(Vector2i(x - 6, y - 3), "log");
		terrainManager.setBlock(Vector2i(x - 5, y - 3), "log");
		terrainManager.setBlock(Vector2i(x - 4, y - 3), "log");
		terrainManager.setBlock(Vector2i(x - 3, y - 3), "log");
		terrainManager.setBlock(Vector2i(x - 2, y - 3), "log");
		terrainManager.setBlock(Vector2i(x - 1, y - 3), "log");
		terrainManager.setBlock(Vector2i(x, y - 3), "log");
		terrainManager.setBlock(Vector2i(x + 1, y - 3), "log");
		terrainManager.setBlock(Vector2i(x + 2, y - 3), "log");
		terrainManager.setBlock(Vector2i(x + 5, y - 3), "log");
		terrainManager.setBlock(Vector2i(x + 2, y - 4), "log");
		terrainManager.setBlock(Vector2i(x + 2, y - 2), "log");

		// Gravels
		// Left
		terrainManager.setBlock(Vector2i(x - 3, y + 1), "gravel");
		terrainManager.setBlock(Vector2i(x - 3, y), "gravel");
		terrainManager.setBlock(Vector2i(x - 3, y - 1), "gravel");
		terrainManager.setBlock(Vector2i(x - 3, y - 2), "gravel");
		terrainManager.setBlock(Vector2i(x - 2, y), "gravel");
		terrainManager.setBlock(Vector2i(x - 2, y - 1), "gravel");
		terrainManager.setBlock(Vector2i(x - 2, y - 2), "gravel");
		terrainManager.setBlock(Vector2i(x - 1, y), "gravel");
		// Right
		terrainManager.setBlock(Vector2i(x + 3, y), "gravel");
		terrainManager.setBlock(Vector2i(x + 3, y - 1), "gravel");
		terrainManager.setBlock(Vector2i(x + 3, y - 2), "gravel");
		terrainManager.setBlock(Vector2i(x + 3, y - 3), "gravel");
		terrainManager.setBlock(Vector2i(x + 3, y - 4), "gravel");
		terrainManager.setBlock(Vector2i(x + 4, y), "gravel");
		terrainManager.setBlock(Vector2i(x + 4, y - 1), "gravel");
		terrainManager.setBlock(Vector2i(x + 4, y - 2), "gravel");
		terrainManager.setBlock(Vector2i(x + 4, y - 3), "gravel");
		terrainManager.setBlock(Vector2i(x + 5, y), "gravel");

		// Planks
		terrainManager.setBlock(Vector2i(x - 7, y + 1), "planks");
		terrainManager.setBlock(Vector2i(x - 6, y + 1), "planks");
		terrainManager.setBlock(Vector2i(x - 5, y + 1), "planks");
		terrainManager.setBlock(Vector2i(x - 4, y + 1), "planks");
		terrainManager.setBlock(Vector2i(x - 2, y + 1), "planks");
		terrainManager.setBlock(Vector2i(x - 1, y + 1), "planks");
		terrainManager.setBlock(Vector2i(x, y + 1), "planks");
		terrainManager.setBlock(Vector2i(x + 1, y + 1), "planks");
		terrainManager.setBlock(Vector2i(x + 2, y + 1), "planks");
		terrainManager.setBlock(Vector2i(x + 3, y + 1), "planks");
		terrainManager.setBlock(Vector2i(x + 4, y + 1), "planks");
		terrainManager.setBlock(Vector2i(x + 5, y + 1), "planks");
		terrainManager.setBlock(Vector2i(x + 6, y + 1), "planks");
		terrainManager.setBlock(Vector2i(x + 7, y + 1), "planks");

		// Torch
		terrainManager.setBlock(Vector2i(x + 1, y - 1), "torch");
		terrainManager.requestLightingUpdate();

		// Chest
		terrainManager.setBlock(Vector2i(x + 2, y), "chest");
		// TODO Items in chest
	}


	// End up wrapping the borders with bedrock
	for (int i = 0; i < width; i++) {
		terrainManager.setBlock(Vector2i(i, 0), "bedrock");
		terrainManager.setBlock(Vector2i(i, height - 1), "bedrock");
	}
	for (int i = 0; i < height; i++) {
		terrainManager.setBlock(Vector2i(0, i), "bedrock");
		terrainManager.setBlock(Vector2i(width - 1, i), "bedrock");
	}

	terrainManager.getSpawnpoints() = spawnPoints;

}


////////////////////////////////////////



