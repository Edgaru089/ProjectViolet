#pragma once

#include "Block.hpp"

class Ladder :public Block {
public:
	const string getBlockId() override { return "ladder"; }
	bool isSolid() override { return false; }
};

