#pragma once

#include "Main.hpp"

class Item;
class Dataset;

class ItemAllocator {
public:
	virtual ~ItemAllocator() {}
	void initalaize();
	shared_ptr<Item> allocate(string id, Dataset& slot, string slotDataIdPrefix, bool hasFocus = false);
public:
	unordered_map<string, function<shared_ptr<Item>(Dataset&, string)>> allocs;
};

ItemAllocator itemAllocator;

template<typename ItemType>
shared_ptr<Item> allocItem(Dataset& slot, string slotDataIdPrefix) { return make_shared<ItemType>(slot, slotDataIdPrefix); }
