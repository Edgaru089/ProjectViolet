#pragma once

#include "Main.hpp"
#include "TerrainManager.hpp"


class TerrainGenerator {
public:

	void setup(Vector2u chunkCount, int spawnCount);

public:

	int getSpawnPointCount() { return spawnCount; }
	const vector<Vector2i>& getSpawnPoints() { return spawnPoints; }

private:

	Vector2u chunkCount;
	int width, height;   // In blocks

	int spawnCount;
	vector<Vector2i> spawnPoints;

};


