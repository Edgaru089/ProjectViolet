#pragma once

#include "BlockAllocator.hpp"

// Include all block headers
#include "Stone.hpp"
#include "LogBlock.hpp"
#include "Torch.hpp"
#include "Gravel.hpp"
#include "Bedrock.hpp"
#include "Planks.hpp"
#include "Chest.hpp"
#include "Ladder.hpp"

#define REGISTER_BLOCK_TYPE(type) allocs.insert(make_pair(type().getBlockId(), alloc<type>))

////////////////////////////////////////
void BlockAllocator::initalaize() {
	REGISTER_BLOCK_TYPE(Stone);
	REGISTER_BLOCK_TYPE(LogBlock);
	REGISTER_BLOCK_TYPE(Torch);
	REGISTER_BLOCK_TYPE(Gravel);
	REGISTER_BLOCK_TYPE(Bedrock);
	REGISTER_BLOCK_TYPE(Planks);
	REGISTER_BLOCK_TYPE(Chest);
	REGISTER_BLOCK_TYPE(Ladder);
}


////////////////////////////////////////
shared_ptr<Block> BlockAllocator::allocate(string id) {
	auto i = allocs.find(id);
	if (i == allocs.end())
		return nullptr;
	else
		return i->second();
}

