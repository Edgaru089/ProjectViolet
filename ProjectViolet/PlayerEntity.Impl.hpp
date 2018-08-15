#pragma once

#include "PlayerEntity.hpp"
#include "TerrainManager.hpp"


////////////////////////////////////////
PlayerEntity::PlayerEntity() :Mob() {
	setData("00item_name", "item_bow"s);
	setData("00count", 1);
	setData("01item_name", "item_minigun"s);
	setData("01count", 1);
	setData("02item_name", "item_rpg_launcher"s);
	setData("02count", 1);
	setData("03item_name", "item_m16a4"s);
	setData("03count", 1);
	setData("04item_name", "item_barrett"s);
	setData("04count", 1);
	setData("05item_name", "item_emx3"s);
	setData("05count", 1);
	setData("06item_name", "item_vtx65"s);
	setData("06count", 1);
	setData("07item_name", "item_mrm36"s);
	setData("07count", 1);
	setData("08item_name", "item_sg55"s);
	setData("08count", 1);

	//slots[1][0].setData("item_name", "item_minigun_ammo"s);
	//slots[1][0].setData("count", 8);
	//slots[1][1].setData("item_name", "item_rpg_ammo"s);
	//slots[1][1].setData("count", 16);
	//slots[1][2].setData("item_name", "item_arrow"s);
	//slots[1][2].setData("count", 64);
	//slots[1][3].setData("item_name", "item_m16a4_ammo"s);
	//slots[1][3].setData("count", 12);
	//slots[1][4].setData("item_name", "item_barrett_ammo"s);
	//slots[1][4].setData("count", 6);
	//slots[1][5].setData("item_name", "item_emx3_ammo"s);
	//slots[1][5].setData("count", 6);
	//slots[1][6].setData("item_name", "item_vtx65_ammo"s);
	//slots[1][6].setData("count", 6);
	setData("17item_name", "item_mrm36_ammo"s);
	setData("17count", 4);
	//slots[1][8].setData("item_name", "item_sg55_ammo"s);
	//slots[1][8].setData("count", 8);

	//slots[2][0].setData("item_name", "item_grenade"s);
	//slots[2][0].setData("count", 8);
	//slots[2][1].setData("item_name", "item_grenade"s);
	//slots[2][1].setData("count", 8);
	//slots[2][2].setData("item_name", "item_grenade"s);
	//slots[2][2].setData("count", 8);
	//slots[2][3].setData("item_name", "item_grenade"s);
	//slots[2][3].setData("count", 8);
	//slots[2][4].setData("item_name", "item_grenade"s);
	//slots[2][4].setData("count", 8);
	//slots[2][5].setData("item_name", "item_grenade"s);
	//slots[2][5].setData("count", 8);
	//slots[2][6].setData("item_name", "item_grenade"s);
	//slots[2][6].setData("count", 8);
	//slots[2][7].setData("item_name", "item_grenade"s);
	//slots[2][7].setData("count", 8);
	//slots[2][8].setData("item_name", "item_grenade"s);
	//slots[2][8].setData("count", 8);

	//slots[3][0].setData("item_name", "block_ladder"s);
	//slots[3][0].setData("count", 5);
	//slots[3][1].setData("item_name", "block_torch"s);
	//slots[3][1].setData("count", 1);
}


////////////////////////////////////////
void PlayerEntity::_pushTriangleVertexes(VertexArray & verts) {
	// Borrow Step I render code from EntityManager::getRenderList()
	TextureInfo tex = getTextureInfo();
	Vector2d center = getPosition()*renderIO.gameScaleFactor;
	double width = getSize().x*renderIO.gameScaleFactor, height = getSize().y*renderIO.gameScaleFactor;
	Vector2i chunk = TerrainManager::convertWorldCoordToChunkId(Vector2i(getPosition().x, getPosition().y));
	Vector2i inCc = TerrainManager::convertWorldCoordToInChunkCoord(Vector2i(getPosition().x, getPosition().y));
	shared_ptr<Chunk> c = terrainManager.getChunk(chunk);
	Uint8 mask;
	if (c != nullptr)
		mask = 63 + 192.0*((double)c->lightLevel[inCc.x][inCc.y] / maxLightingLevel);
	else
		mask = 63;

	// Left-Top
	verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y - height),
						Color(mask, mask, mask),
						Vector2f(tex.textureRect.left, tex.textureRect.top)));
	// Right-Top
	verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - height),
						Color(mask, mask, mask),
						Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top)));
	// Left-Bottom
	verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y),
						Color(mask, mask, mask),
						Vector2f(tex.textureRect.left, tex.textureRect.top + tex.textureRect.height)));
	// Right-Top
	verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - height),
						Color(mask, mask, mask),
						Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top)));
	// Left-Bottom
	verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y),
						Color(mask, mask, mask),
						Vector2f(tex.textureRect.left, tex.textureRect.top + tex.textureRect.height)));
	// Right-Bottom
	verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y),
						Color(mask, mask, mask),
						Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top + tex.textureRect.height)));

	// Step II - Item in Hand
	//if (isLocalPlayer()) {
	//	Dataset& data = playerInventory.slots[0][playerInventory.getCursorId()];
	//	string& name = data["item_name"].getDataString();
	//	if (name != "") {
	//		if (name.substr(0, 4) == "item") {
	//			Item* i = itemAllocator.allocate(name.substr(5), data, true);
	//			tex = i->getTextureInfo(); delete i;
	//		}
	//		else if (name.substr(0, 5) == "block") {
	//			Block* i = blockAllocator.allocate(name.substr(6));
	//			tex = i->getTextureInfo(); delete i;
	//		}
	//		else
	//			tex = textureManager.getTextureInfo("none");

	//		double offset = 0.2, itemSize = 1.2;
	//		Transform trans;
	//		Vector2d center = getEyePosition();
	//		trans.rotate(gameIO.degreeAngle + 135.0, Vector2f(center));
	//		bool flip = gameIO.degreeAngle > 90.0&&gameIO.degreeAngle < 270.0;
	//		// Left-Top     (with rotation = 0)
	//		verts.append(Vertex(trans.transformPoint(Vector2f(center.x - itemSize / 2.0, center.y - itemSize / 2.0))*(float)renderIO.gameScaleFactor,
	//							Color(mask, mask, mask),
	//							Vector2f(tex.textureRect.left, 0 + tex.textureRect.top)));
	//		// Right-Top    (with rotation = 0)
	//		verts.append(Vertex(trans.transformPoint(Vector2f(center.x + itemSize / 2.0, center.y - itemSize / 2.0))*(float)renderIO.gameScaleFactor,
	//							Color(mask, mask, mask),
	//							Vector2f(tex.textureRect.left + tex.textureRect.width, 0 + tex.textureRect.top)));
	//		// Left-Bottom  (with rotation = 0)
	//		verts.append(Vertex(trans.transformPoint(Vector2f(center.x - itemSize / 2.0, center.y + itemSize / 2.0))*(float)renderIO.gameScaleFactor,
	//							Color(mask, mask, mask),
	//							Vector2f(tex.textureRect.left, tex.textureRect.top + tex.textureRect.height)));
	//		// Right-Top    (with rotation = 0)
	//		verts.append(Vertex(trans.transformPoint(Vector2f(center.x + itemSize / 2.0, center.y - itemSize / 2.0))*(float)renderIO.gameScaleFactor,
	//							Color(mask, mask, mask),
	//							Vector2f(tex.textureRect.left + tex.textureRect.width, 0 + tex.textureRect.top)));
	//		// Left-Bottom  (with rotation = 0)
	//		verts.append(Vertex(trans.transformPoint(Vector2f(center.x - itemSize / 2.0, center.y + itemSize / 2.0))*(float)renderIO.gameScaleFactor,
	//							Color(mask, mask, mask),
	//							Vector2f(tex.textureRect.left, tex.textureRect.top + tex.textureRect.height)));
	//		// Right-Bottom (with rotation = 0)
	//		verts.append(Vertex(trans.transformPoint(Vector2f(center.x + itemSize / 2.0, center.y + itemSize / 2.0))*(float)renderIO.gameScaleFactor,
	//							Color(mask, mask, mask),
	//							Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top + tex.textureRect.height)));
	//	}
	//}
}


////////////////////////////////////////
void PlayerEntity::_updateLogic() {
	if (onLadder()) {
		onGround = true;
		vecY = 0;
		if (isAscendingLadder())
			vecY += -getMaxSpeed()*0.8;
		if (isDecendingLadder())
			vecY += getMaxSpeed()*0.8;
		shared_ptr<Block> b = terrainManager.getBlock(Vector2i(posX, posY));
		if (b == nullptr || b->getBlockId() != "ladder") {
			onLadder() = false;
			isAscendingLadder() = false;
			isDecendingLadder() = false;
		}
	}

	if (isMovingLeft())
		if (onLadder())
			accelerate(Vector2d(-65 * logicIO.deltaTime.asSeconds(), 0));
		else if (onGround)
			accelerate(Vector2d(-75 * logicIO.deltaTime.asSeconds(), 0));
		else
			accelerate(Vector2d(-4.0*logicIO.deltaTime.asSeconds(), 0));
	if (isMovingRight())
		if (onLadder())
			accelerate(Vector2d(65 * logicIO.deltaTime.asSeconds(), 0));
		else if (onGround)
			accelerate(Vector2d(75 * logicIO.deltaTime.asSeconds(), 0));
		else
			accelerate(Vector2d(4.0*logicIO.deltaTime.asSeconds(), 0));

	if (vecX > getMaxSpeed())
		vecX = getMaxSpeed();
	if (vecX < -getMaxSpeed())
		vecX = -getMaxSpeed();

	if (wantStandup()) {
		bool ok = true;

		// Borrow collision code from Entity
		vector<Vector2d> points;
		crouched() = false;
		DoubleRect bound = getHitbox();
		crouched() = true;

		points.push_back(Vector2d(bound.left, bound.top));                 // Top-Left  Point (X-Negative, Y-Negative)
		points.push_back(Vector2d(bound.left + bound.width, bound.top));   // Top-Right Point (X-Positive, Y-Negative)

		for (Vector2d i : points) {
			shared_ptr<Block> top = terrainManager.getBlock(TerrainManager::convertWorldPositionToBlockCoord(i));
			if (top != nullptr && top->isSolid()) {
				ok = false;
				break;
			}
		}

		if (ok) {
			wantStandup() = false;
			crouched() = false;
		}
	}

	// Collect Item
	for (auto& i : entityManager.getEntityMapList()) {
		if (i.second->getEntityId() == "item_entity") {
			if (getHitbox().intersects(i.second->getHitbox())) {
				try {
					collectItem(dynamic_cast<ItemEntity&>(*i.second));
				}
				catch (bad_cast) {}
			}
		}
	}
}


////////////////////////////////////////
void PlayerEntity::jump() {
	if (isOnLadder())
		jumpOffLadder();
	else if (onGround)
		accelerate(Vector2d(0, -6));
}


////////////////////////////////////////
void PlayerEntity::moveLeft(bool state) {
	isMovingLeft() = state;
}


////////////////////////////////////////
void PlayerEntity::moveRight(bool state) {
	isMovingRight() = state;
}


////////////////////////////////////////
bool PlayerEntity::crouch(bool state) {
	crouched() = state;
	wantStandup() = false;
	if (!state) {
		bool ok = true;

		// Borrow collision code from Entity
		vector<Vector2d> points;
		crouched() = false;
		DoubleRect bound = getHitbox();
		crouched() = state;

		points.push_back(Vector2d(bound.left, bound.top));                 // Top-Left  Point (X-Negative, Y-Negative)
		points.push_back(Vector2d(bound.left + bound.width, bound.top));   // Top-Right Point (X-Positive, Y-Negative)

		for (Vector2d i : points) {
			shared_ptr<Block> top = terrainManager.getBlock(TerrainManager::convertWorldPositionToBlockCoord(i));
			if (top != nullptr && top->isSolid()) {
				ok = false;
				break;
			}
		}

		if (ok) {
			wantStandup() = false;
			crouched() = false;
		}
		else {
			wantStandup() = true;
			crouched() = true;
		}
	}
	return crouched();
}


////////////////////////////////////////
void PlayerEntity::ascendLadder(bool state) {
	shared_ptr<Block> b = terrainManager.getBlock(Vector2i(posX, posY));
	if (b != nullptr && b->getBlockId() == "ladder") {
		onLadder() = true;
		isAscendingLadder() = state;
	}
}


////////////////////////////////////////
void PlayerEntity::decendLadder(bool state) {
	shared_ptr<Block> b = terrainManager.getBlock(Vector2i(posX, posY));
	if (b != nullptr && b->getBlockId() == "ladder") {
		onLadder() = true;
		isDecendingLadder() = state;
	}
}


////////////////////////////////////////
void PlayerEntity::jumpOffLadder() {
	vecY = 0;
	onLadder() = false;
}


////////////////////////////////////////
bool PlayerEntity::collectItem(ItemEntity& item) {
	//TODO Collect Item

	// First attempt to merge with items which exist
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 9; j++) {
			Dataset& d = getDataset();
			if (d[to_string(i) + to_string(j) + "count"].getDataInt() < maxItemsPerSlot && d[to_string(i) + to_string(j) + "item_name"].getDataString() == item.getItemName()) {
				d[to_string(i) + to_string(j) + "count"].getDataInt()++;
				item.kill();
				return true;
			}
		}
	// Then try to place at an empty slot
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 9; j++) {
			Dataset& d = getDataset();
			if (d[to_string(i) + to_string(j) + "item_name"].getDataString() == "") {
				d[to_string(i) + to_string(j) + "item_name"].setData(item.getItemName());
				d[to_string(i) + to_string(j) + "count"].getDataInt() = 1;
				for (auto& i : item.getDataset().getDatasets())
					d.getDatasets().insert(i);
				item.kill();
				return true;
			}
		}

	return false;
}

