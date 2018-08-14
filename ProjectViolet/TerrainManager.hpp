#pragma once

#include <utility>
#include "Main.hpp"
#include "Chunk.hpp"
#include "EntityManager.hpp"


class TerrainManager : public Lockable {
public:

	static Vector2i convertWorldCoordToChunkId(Vector2i coord);
	static Vector2i convertWorldCoordToInChunkCoord(Vector2i coord);
	static Vector2i convertChunkToWorldCoord(Vector2i chunkId, Vector2i inChunkCoord);
	static Vector2i convertWorldPositionToBlockCoord(Vector2d position);

	static Vector2i convertScreenPixelToWorldBlockCoord(Vector2i pixel);
	static Vector2d convertScreenPixelToWorldCoord(Vector2i pixel);

public:

	~TerrainManager();

	void updateLogic();
	void getRenderList(VertexArray& array); // Triangles
	void getLightMask(VertexArray& array); // Triangles

	shared_ptr<Chunk> loadEmptyChunk(Vector2i id);

	void unloadChunk(Vector2i id);

	// Just stores the data; never uses it
	void setChunkCount(Vector2i chunkCount) { this->chunkCount = chunkCount; }
	Vector2i getChunkCount() { return chunkCount; }

	vector<Vector2i>& getSpawnpoints() { return spawns; }

	void clearChunks();

	shared_ptr<Chunk> getChunk(Vector2i chunkId);
	map<Vector2i, shared_ptr<Chunk>, Vector2Less<int>>& getChunks() { return chunks; }
	shared_ptr<Block> getBlock(Vector2i coord);

	// SetBlock() doesn't call the block's OnDestroy() function
	// Use PlaceBlock() when player places a block
	void setBlock(Vector2i coord, string blockId);

public:

	// Destroy/Break a block
	void breakBlock(Vector2i pos, Entity * breaker = nullptr);

	// Place a block
	// Does nothing if the block isn't empty
	void placeBlock(Vector2i pos, string blockId, Entity * placer = nullptr, bool isForced = false);

	// Request a whole-map lighting update
	void requestLightingUpdate() { wantUpdateLight = true; }

private:

	void _updateLighting();


	map<Vector2i, shared_ptr<Chunk>, Vector2Less<int>> chunks;

	bool wantUpdateLight;

	// Just stores the data; never uses it
	Vector2i chunkCount;
	vector<Vector2i> spawns;
};


TerrainManager terrainManager;
