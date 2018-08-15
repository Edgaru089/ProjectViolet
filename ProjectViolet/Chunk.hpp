#pragma once

#include <vector>
#include "Main.hpp"
#include "TextureManager.hpp"
#include "Data.hpp"
#include "Block.hpp"


const int chunkSize = 16;

class Chunk {
public:

	friend class TerrainManager;

	~Chunk();

	void updateLogic();

	void getRenderList(VertexArray & verts);
	void getLightMask(VertexArray& verts);

	void setChunkId(int x, int y);
	Vector2i getChunkId();

	shared_ptr<Block> getBlock(Vector2i inChunkCoord);
	void setBlock(Vector2i inChunkCoord, shared_ptr<Block> block);

	void _resize(int width, int height);

	vector<vector<shared_ptr<Block>>> blocks;
	vector<vector<int>> lightLevel;
	Vector2i id;

};

