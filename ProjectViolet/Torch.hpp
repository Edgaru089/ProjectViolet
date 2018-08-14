#pragma once

#include "Main.hpp"
#include "Block.hpp"

class Torch :public Block {
public:

	Torch() {}

	const string getBlockId() override { return "torch"; }
	bool isSolid() override { return false; }
	int getLightStrength() override { return 12; }

private:

};
