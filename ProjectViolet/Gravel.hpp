#pragma once

#include "Block.hpp"


class Gravel :public Block {
public:

	const string getBlockId() override { return "gravel"; }

private:


};

