#pragma once

#include "Main.hpp"

class Item;
class Dataset;

class ItemAllocator {
public:
	virtual ~ItemAllocator() {}
	void initalaize();
	shared_ptr<Item> allocate(string id, Dataset& slot, bool hasFocus = false);
private:
	map<string, function<shared_ptr<Item>(Dataset&)>> allocs;
};

ItemAllocator itemAllocator;

template<typename ItemType>
shared_ptr<Item> allocItem(Dataset& slot) { return make_shared<ItemType>(slot); }
