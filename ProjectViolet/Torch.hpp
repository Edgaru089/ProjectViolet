#pragma once

#include "Main.hpp"
#include "Block.hpp"

class Torch :public Block {
public:

	Torch() {}

	const string getBlockId() override { return "torch"; }
	bool isSolid() override { return false; }
	int getLightSourceStrength() override { return 12; }

private:

};
