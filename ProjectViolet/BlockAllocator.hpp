#pragma once

#include "Main.hpp"

class Block;

class BlockAllocator {
public:
	virtual ~BlockAllocator() {}
	void initalaize();
	shared_ptr<Block> allocate(string id);
public:
	unordered_map<string, function<shared_ptr<Block>(void)>> allocs;
};

BlockAllocator blockAllocator;

template<typename BlockType>
shared_ptr<Block> alloc() { return make_shared<BlockType>(); }
