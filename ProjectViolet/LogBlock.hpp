#pragma once

#include "Main.hpp"
#include "Block.hpp"


class LogBlock :public Block {
public:

	const string getBlockId() override { return "log"; }

private:

};
