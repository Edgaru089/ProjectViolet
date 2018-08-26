#pragma once

#include "Main.hpp"
#include "Block.hpp"
#include "InventoryObject.hpp"


class Chest :public Block, public InventoryObject {
public:

	Chest();

	const string getBlockId() override { return "chest"; }
	DoubleRect getHitbox() override;
	const Vector2i getInventorySize() override { return Vector2i(4, 9); }

	const bool requestSpeicalRendering() override { return true; }
	void _pushTriangleVertexes(VertexArray& verts) override;

	void _onRightClick() override;

};
