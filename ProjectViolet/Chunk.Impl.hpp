
#include "Chunk.hpp"


////////////////////////////////////////
Chunk::~Chunk() {
}


////////////////////////////////////////
void Chunk::updateLogic() {
	for (auto& i : blocks)
		for (shared_ptr<Block> j : i)
			if (j != nullptr)
				j->updateLogic();
}


////////////////////////////////////////
void Chunk::getRenderList(VertexArray& verts) {
	for (int i = 0; i < chunkSize; i++)
		for (int j = 0; j < chunkSize; j++) {
			if (blocks[i][j] != nullptr && blocks[i][j]->getHitbox().intersects(renderIO.viewRectBlock)) {
				if (blocks[i][j]->requestSpeicalRendering()) {
					blocks[i][j]->_pushTriangleVertexes(verts);
					continue;
				}

				Vector2d center = Vector2d(i + id.x*chunkSize + 0.5, j + id.y*chunkSize + 0.5)*renderIO.gameScaleFactor;
				double width = renderIO.gameScaleFactor;

				Uint8 mask = 63 + 192.0*((double)lightLevel[i][j] / maxLightingLevel);
				TextureInfo tex = blocks[i][j]->getTextureInfo();
				// Left-Top
				verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y - width / 2.0),
					Vector2f(tex.textureRect.left, tex.textureRect.top)));
				// Right-Top
				verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - width / 2.0),
					Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top)));
				// Left-Bottom
				verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y + width / 2.0),
					Vector2f(tex.textureRect.left, tex.textureRect.top + tex.textureRect.height)));
				// Right-Top
				verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - width / 2.0),
					Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top)));
				// Left-Bottom
				verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y + width / 2.0),
					Vector2f(tex.textureRect.left, tex.textureRect.top + tex.textureRect.height)));
				// Right-Bottom
				verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y + width / 2.0),
					Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top + tex.textureRect.height)));
			}
		}
	verts.setPrimitiveType(PrimitiveType::Triangles);
}


////////////////////////////////////////
void Chunk::getLightMask(VertexArray& verts) {
	for (int i = 0; i < chunkSize; i++)
		for (int j = 0; j < chunkSize; j++) {

			Vector2d center = Vector2d(i + id.x*chunkSize + 0.5, j + id.y*chunkSize + 0.5)*renderIO.gameScaleFactor;
			double width = renderIO.gameScaleFactor;

			if (!renderIO.viewRectBlock.intersects(DoubleRect(
				i + id.x*chunkSize, j + id.y*chunkSize, 1, 1)))
				continue;

			Uint8 mask = 63 + 192.0*((double)lightLevel[i][j] / maxLightingLevel);

			// Mask
			// Left-Top
			verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y - width / 2.0),
				Color(0, 0, 0, 255 - mask)));
			// Right-Top
			verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - width / 2.0),
				Color(0, 0, 0, 255 - mask)));
			// Left-Bottom
			verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y + width / 2.0),
				Color(0, 0, 0, 255 - mask)));
			// Right-Top
			verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - width / 2.0),
				Color(0, 0, 0, 255 - mask)));
			// Left-Bottom
			verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y + width / 2.0),
				Color(0, 0, 0, 255 - mask)));
			// Right-Bottom
			verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y + width / 2.0),
				Color(0, 0, 0, 255 - mask)));

		}
	verts.setPrimitiveType(PrimitiveType::Triangles);
}


////////////////////////////////////////
void Chunk::setChunkId(int x, int y) {
	id = Vector2i(x, y);
}


////////////////////////////////////////
Vector2i Chunk::getChunkId() {
	return id;
}


////////////////////////////////////////
shared_ptr<Block> Chunk::getBlock(Vector2i inChunkCoord) {
	if (inChunkCoord.x < 0 || inChunkCoord.x >= chunkSize || inChunkCoord.y < 0 || inChunkCoord.y >= chunkSize)
		return nullptr;
	return blocks[inChunkCoord.x][inChunkCoord.y];
}


////////////////////////////////////////
void Chunk::setBlock(Vector2i inChunkCoord, shared_ptr<Block> block) {
	blocks[inChunkCoord.x][inChunkCoord.y] = block;
	if (block != nullptr) {
		block->setInChunkPosition(inChunkCoord);
		block->setChunkId(id);
	}
}


////////////////////////////////////////
void Chunk::_resize(int width, int height) {
	blocks.resize(width);
	blocks.shrink_to_fit();
	for (auto& i : blocks) {
		i.resize(height);
		i.shrink_to_fit();
	}
	lightLevel.resize(width);
	lightLevel.shrink_to_fit();
	for (auto& i : lightLevel) {
		i.resize(height);
		i.shrink_to_fit();
	}
}
