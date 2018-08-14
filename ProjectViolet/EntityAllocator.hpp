#pragma once

#include "Main.hpp"

class Entity;

class EntityAllocator {
public:
	void initalaize();
	shared_ptr<Entity> allocate(string id);
private:
	map<string, function<shared_ptr<Entity>(void)>> allocs;
};

EntityAllocator entityAllocator;

template<typename EntityType>
shared_ptr<EntityType> allocEntity() { return make_shared<EntityType>(); }
