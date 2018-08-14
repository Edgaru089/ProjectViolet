#pragma once

#include "Main.hpp"
#include "Block.hpp"
#include "Entity.hpp"


class Stone :public Block {
public:

	const string getBlockId() override { return "stone"; }

private:

};
