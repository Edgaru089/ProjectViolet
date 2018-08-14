#pragma once

#include "Block.hpp"

class Bedrock :public Block {
public:
	const string getBlockId() override { return "bedrock"; }
};
