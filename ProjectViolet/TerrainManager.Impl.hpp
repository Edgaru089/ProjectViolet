
#include "TerrainManager.hpp"
#include "IO.hpp"


////////////////////////////////////////
Vector2i TerrainManager::convertWorldCoordToChunkId(Vector2i coord) {
	int X, Y;

	if (coord.x >= 0)
		X = coord.x / chunkSize;
	else
		X = -((-coord.x - 1) / chunkSize) - 1;

	if (coord.y >= 0)
		Y = coord.y / chunkSize;
	else
		Y = -((-coord.y - 1) / chunkSize) - 1;

	return Vector2i(X, Y);
}


////////////////////////////////////////
Vector2i TerrainManager::convertWorldCoordToInChunkCoord(Vector2i coord) {
	int X, Y;

	if (coord.x >= 0)
		X = coord.x % chunkSize;
	else
		X = chunkSize - ((-coord.x - 1) % chunkSize) - 1;

	if (coord.y >= 0)
		Y = coord.y % chunkSize;
	else
		Y = chunkSize - ((-coord.y - 1) % chunkSize) - 1;

	return Vector2i(X, Y);
}


////////////////////////////////////////
Vector2i TerrainManager::convertChunkToWorldCoord(Vector2i chunkId, Vector2i inChunkCoord) {
	return chunkId * chunkSize + inChunkCoord;
}


////////////////////////////////////////
Vector2i TerrainManager::convertWorldPositionToBlockCoord(Vector2d position) {
	int X, Y;
	X = position.x;
	Y = position.y;
	if (position.x < 0)
		X--;
	if (position.y < 0)
		Y--;
	return Vector2i(X, Y);
}


////////////////////////////////////////
Vector2i TerrainManager::convertScreenPixelToWorldBlockCoord(Vector2i pixel) {
	int X, Y;

	if (pixel.x - renderIO.gameScenePosOffset.x >= 0)
		X = (pixel.x - renderIO.gameScenePosOffset.x) / renderIO.gameScaleFactor;
	else
		X = (pixel.x - renderIO.gameScenePosOffset.x) / renderIO.gameScaleFactor - 1;

	if (pixel.y - renderIO.gameScenePosOffset.y >= 0)
		Y = (pixel.y - renderIO.gameScenePosOffset.y) / renderIO.gameScaleFactor;
	else
		Y = (pixel.y - renderIO.gameScenePosOffset.y) / renderIO.gameScaleFactor - 1;

	return Vector2i(X, Y);
}


////////////////////////////////////////
Vector2d TerrainManager::convertScreenPixelToWorldCoord(Vector2i pixel) {
	double X, Y;

	if (pixel.x - renderIO.gameScenePosOffset.x >= 0)
		X = (pixel.x - renderIO.gameScenePosOffset.x) / renderIO.gameScaleFactor;
	else
		X = (pixel.x - renderIO.gameScenePosOffset.x) / renderIO.gameScaleFactor - 1;

	if (pixel.y - renderIO.gameScenePosOffset.y >= 0)
		Y = (pixel.y - renderIO.gameScenePosOffset.y) / renderIO.gameScaleFactor;
	else
		Y = (pixel.y - renderIO.gameScenePosOffset.y) / renderIO.gameScaleFactor - 1;

	return Vector2d(X, Y);
}


////////////////////////////////////////
TerrainManager::~TerrainManager() {
}


////////////////////////////////////////
void TerrainManager::updateLogic() {
	AUTOLOCKABLE(*this);
	for (auto&i : chunks)
		i.second->updateLogic();
	if (wantUpdateLight) {
		_updateLighting();
		wantUpdateLight = false;
	}
}


////////////////////////////////////////
void TerrainManager::getRenderList(VertexArray& array) {
	AUTOLOCKABLE(*this);
	for (auto&i : chunks) {
		i.second->getRenderList(array);
	}
}


////////////////////////////////////////
void TerrainManager::getLightMask(VertexArray& array) {
	AUTOLOCKABLE(*this);
	for (auto&i : chunks)
		i.second->getLightMask(array);

	// Append extra triangles covering the outside if required
	// Left (and two corners on the left)
	Vector2d rightBottomT = Vector2d(getChunkCount()) * 16.0 * renderIO.gameScaleFactor;
	if (renderIO.viewRect.left < 0) {
		array.append(Vertex(Vector2f(0, renderIO.viewRect.top), Color(0, 0, 0, 192))); // Right-Top
		array.append(Vertex(Vector2f(renderIO.viewRect.left, renderIO.viewRect.top), Color(0, 0, 0, 192))); // Left-Top
		array.append(Vertex(Vector2f(0, renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Right-Bottom
		array.append(Vertex(Vector2f(renderIO.viewRect.left, renderIO.viewRect.top), Color(0, 0, 0, 192))); // Left-Top
		array.append(Vertex(Vector2f(0, renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Right-Bottom
		array.append(Vertex(Vector2f(renderIO.viewRect.left, renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Left-Bottom
	}
	// Right (and two corners on the right)
	if (renderIO.viewRect.left + renderIO.viewRect.width > rightBottomT.x) {
		array.append(Vertex(Vector2f(renderIO.viewRect.left + renderIO.viewRect.width, renderIO.viewRect.top), Color(0, 0, 0, 192))); // Right-Top
		array.append(Vertex(Vector2f(rightBottomT.x, renderIO.viewRect.top), Color(0, 0, 0, 192))); // Left-Top
		array.append(Vertex(Vector2f(renderIO.viewRect.left + renderIO.viewRect.width, renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Right-Bottom
		array.append(Vertex(Vector2f(rightBottomT.x, renderIO.viewRect.top), Color(0, 0, 0, 192))); // Left-Top
		array.append(Vertex(Vector2f(renderIO.viewRect.left + renderIO.viewRect.width, renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Right-Bottom
		array.append(Vertex(Vector2f(rightBottomT.x, renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Left-Bottom
	}
	// Top
	if (renderIO.viewRect.top < 0) {
		array.append(Vertex(Vector2f(min(renderIO.viewRect.left + renderIO.viewRect.width, rightBottomT.x), renderIO.viewRect.top), Color(0, 0, 0, 192))); // Right-Top
		array.append(Vertex(Vector2f(max(renderIO.viewRect.left, 0), renderIO.viewRect.top), Color(0, 0, 0, 192))); // Left-Top
		array.append(Vertex(Vector2f(min(renderIO.viewRect.left + renderIO.viewRect.width, rightBottomT.x), 0), Color(0, 0, 0, 192))); // Right-Bottom
		array.append(Vertex(Vector2f(max(renderIO.viewRect.left, 0), renderIO.viewRect.top), Color(0, 0, 0, 192))); // Left-Top
		array.append(Vertex(Vector2f(min(renderIO.viewRect.left + renderIO.viewRect.width, rightBottomT.x), 0), Color(0, 0, 0, 192))); // Right-Bottom
		array.append(Vertex(Vector2f(max(renderIO.viewRect.left, 0), 0), Color(0, 0, 0, 192))); // Left-Bottom
	}
	// Bottom
	if (renderIO.viewRect.top + renderIO.viewRect.height > rightBottomT.y) {
		array.append(Vertex(Vector2f(min(renderIO.viewRect.left + renderIO.viewRect.width, rightBottomT.x), rightBottomT.y), Color(0, 0, 0, 192))); // Right-Top
		array.append(Vertex(Vector2f(max(renderIO.viewRect.left, 0), rightBottomT.y), Color(0, 0, 0, 192))); // Left-Top
		array.append(Vertex(Vector2f(min(renderIO.viewRect.left + renderIO.viewRect.width, rightBottomT.x), renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Right-Bottom
		array.append(Vertex(Vector2f(max(renderIO.viewRect.left, 0), rightBottomT.y), Color(0, 0, 0, 192))); // Left-Top
		array.append(Vertex(Vector2f(min(renderIO.viewRect.left + renderIO.viewRect.width, rightBottomT.x), renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Right-Bottom
		array.append(Vertex(Vector2f(max(renderIO.viewRect.left, 0), renderIO.viewRect.top + renderIO.viewRect.height), Color(0, 0, 0, 192))); // Left-Bottom
	}
}


////////////////////////////////////////
shared_ptr<Chunk> TerrainManager::loadEmptyChunk(Vector2i id) {
	AUTOLOCKABLE(*this);
	auto i = chunks.insert(make_pair(id, make_shared<Chunk>())).first;
	i->second->id = id;
	i->second->setChunkId(id.x, id.y);
	i->second->blocks.resize(chunkSize);
	i->second->lightLevel.resize(chunkSize);
	for (int j = 0; j < chunkSize; j++) {
		i->second->blocks[j].resize(chunkSize, nullptr);
		i->second->lightLevel[j].resize(chunkSize, 0);
	}
	return i->second;
}


////////////////////////////////////////
void TerrainManager::unloadChunk(Vector2i id) {
	AUTOLOCKABLE(*this);
	map<Vector2i, shared_ptr<Chunk>, Vector2Less<int>>::iterator i;
	if ((i = chunks.find(id)) != chunks.end()) {
		chunks.erase(i);
	}
}


////////////////////////////////////////
void TerrainManager::clearChunks() {
	AUTOLOCKABLE(*this);

	chunks.clear();
}


////////////////////////////////////////
shared_ptr<Chunk> TerrainManager::getChunk(Vector2i chunkId) {
	AUTOLOCKABLE(*this);
	auto i = chunks.find(chunkId);
	if (i != chunks.end())
		return i->second;
	else
		return nullptr;
}


////////////////////////////////////////
shared_ptr<Block> TerrainManager::getBlock(Vector2i coord) {
	AUTOLOCKABLE(*this);
	auto i = chunks.find(convertWorldCoordToChunkId(coord));
	if (i != chunks.end() && i->second != nullptr)
		return i->second->getBlock(convertWorldCoordToInChunkCoord(coord));
	else
		return nullptr;
}


////////////////////////////////////////
void TerrainManager::setBlock(Vector2i coord, string blockId) {
	AUTOLOCKABLE(*this);
	if (chunks.find(convertWorldCoordToChunkId(coord)) != chunks.end()) {
		chunks[convertWorldCoordToChunkId(coord)]->setBlock(
			convertWorldCoordToInChunkCoord(coord), blockAllocator.allocate(blockId));
	}
}


////////////////////////////////////////
void TerrainManager::breakBlock(Vector2i pos, Entity * breaker) {
	AUTOLOCKABLE(*this);
	auto i = chunks.find(convertWorldCoordToChunkId(pos));
	if (i == chunks.end()) // Chunk not loaded
		return;

	shared_ptr<Chunk> c = i->second;
	shared_ptr<Block> b = c->getBlock(convertWorldCoordToInChunkCoord(pos));
	if (b != nullptr) {
		b->onDestroy(breaker);
		if (b->getLightStrength() > 0)
			wantUpdateLight = true;
		setBlock(pos, ""); // nullptr means empty
	}
}


////////////////////////////////////////
void TerrainManager::placeBlock(Vector2i pos, string blockId, Entity * placer, bool isForced) {
	AUTOLOCKABLE(*this);
	map<Vector2i, shared_ptr<Chunk>, Vector2Less<int>>::iterator i = chunks.find(convertWorldCoordToChunkId(pos));
	if (i == chunks.end() || i->second == nullptr)
		return;
	if (i->second->getBlock(convertWorldCoordToInChunkCoord(pos)) == nullptr) {
		shared_ptr<Block> b = blockAllocator.allocate(blockId);
		i->second->setBlock(convertWorldCoordToInChunkCoord(pos), b);
		b->_onPlaced(placer);
		if (b->getLightStrength() > 0)
			wantUpdateLight = true;
	}
}


////////////////////////////////////////
void TerrainManager::_updateLighting() {
	AUTOLOCKABLE(*this);

	vector<pair<Vector2i, int>> lights;

	for (auto& k : chunks) {
		auto c = k.second;
		for (int i = 0; i < chunkSize; i++)
			for (int j = 0; j < chunkSize; j++)
				if (c->getBlock(Vector2i(i, j)) != nullptr &&
					c->getBlock(Vector2i(i, j))->getLightStrength() > 0)
					lights.push_back(make_pair(convertChunkToWorldCoord(k.first, Vector2i(i, j)),
											   c->getBlock(Vector2i(i, j))->getLightStrength()));
	}

	for (auto& l : chunks) {
		auto c = l.second;
		for (int i = 0; i < chunkSize; i++)
			for (int j = 0; j < chunkSize; j++) {
				Vector2i coord = convertChunkToWorldCoord(l.first, Vector2i(i, j));
				int Max = 0;
				for (auto& k : lights) {
					int level;
					if (k.first == coord)
						level = k.second;
					else
						level = k.second - (abs(k.first.x - coord.x) + abs(k.first.y - coord.y));
					Max = max(Max, level);
				}
				c->lightLevel[i][j] = Max;
			}
	}
}

