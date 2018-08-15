#pragma once

#include <string>
#include <map>
#include "Main.hpp"

#include "TextureManager.hpp"
#include "Data.hpp"
#include "Chunk.hpp"
#include "BlockAllocator.hpp"

class Entity;

const int maxLightingLevel = 12;

class Block {
public:

	friend class TerrainManager;

	virtual ~Block() {}

	virtual const string getBlockId() = 0;

	string getTextureId() { return "block_" + getBlockId(); }
	TextureInfo getTextureInfo() { return textureManager.getTextureInfo(getTextureId()); }

	virtual const bool requestSpeicalRendering() { return false; }
	virtual void _pushTriangleVertexes(VertexArray& arr) {}

	virtual bool isSolid() { return true; }

	void updateLogic();

	// Zerom means none
	virtual int getLightStrength() { return 0; }

	Data& getData(string id) {
		return datasets[id];
	}

	template<typename Type>
	void setData(string id, Type data) {
		datasets[id].setData(data);
	}

	Dataset& getDataset() { return datasets; }

	Vector2i getInChunkPosition() { return inChunkPos; }
	void setInChunkPosition(int x, int y) { inChunkPos.x = x; inChunkPos.y = y; }
	void setInChunkPosition(Vector2i pos) { this->inChunkPos = pos; }

	// Global block coords
	Vector2i getPosition();

	virtual DoubleRect getHitbox();

	Vector2i getChunkId() { return chunk; }
	void setChunkId(Vector2i chunkId) { chunk = chunkId; }

public:

	virtual void _updateLogic() {}
	virtual void _onCollision(Entity* entity) {}
	virtual void _onRightClick() {}

protected:

	Vector2i inChunkPos, chunk;

	Dataset datasets;

};
