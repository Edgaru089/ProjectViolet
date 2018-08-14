#pragma once

#include "Main.hpp"
#include "Block.hpp"


class Planks :public Block {
public:
	const string getBlockId() override { return "planks"; }
};
