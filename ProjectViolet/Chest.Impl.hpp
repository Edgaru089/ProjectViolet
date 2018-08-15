#pragma once

#include "Chest.hpp"
#include "TerrainManager.hpp"
#include "InGameUI.hpp"
#include "ChestInventory.hpp"


////////////////////////////////////////
Chest::Chest() {
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 9; j++) {
			setData(to_string(i) + to_string(j) + "item_name", ""s);
			setData(to_string(i) + to_string(j) + "count", 0);
		}
}


////////////////////////////////////////
DoubleRect Chest::getHitbox() {
	Vector2i globalPos = TerrainManager::convertChunkToWorldCoord(chunk, inChunkPos);
	return DoubleRect(globalPos.x + 0.0625, globalPos.y + 0.125, 0.875, 0.875);
}


////////////////////////////////////////
void Chest::_pushTriangleVertexes(VertexArray& verts) {
	int lightLevel = terrainManager.getChunk(chunk)->lightLevel[inChunkPos.x][inChunkPos.y];
	Vector2d center = Vector2d(inChunkPos.x + chunk.x*chunkSize + 0.5,
							   inChunkPos.y + chunk.y*chunkSize + 0.5 + 0.0625)*renderIO.gameScaleFactor;
	double width = renderIO.gameScaleFactor*0.875;

	Uint8 mask = 63 + 192.0*((double)lightLevel / maxLightingLevel);
	TextureInfo tex = getTextureInfo();
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


////////////////////////////////////////
void Chest::_onRightClick() {
	uiManager.changeUI(make_shared<ChestInventory>(getPosition()));
}

