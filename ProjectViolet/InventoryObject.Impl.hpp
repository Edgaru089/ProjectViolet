#pragma once

#include "InventoryObject.hpp"
#include "ItemEntity.hpp"
#include "EntityManager.hpp"


////////////////////////////////////////
void InventoryObject::dropAllItems(Dataset& dataset, Vector2d centerPos) {
	// Tile Drop
	Vector2i s = getInventorySize();
	for (int i = 0; i < s.x; i++)
		for (int j = 0; j < s.y; j++) {
			string prefix = to_string(i) + to_string(j);
			string& name = dataset[prefix + "item_name"].getDataString();
			int& count = dataset[prefix + "count"].getDataInt();
			if (name.empty() || count <= 0)
				continue;
			shared_ptr<ItemEntity> e = make_shared<ItemEntity>(name);
			for (auto& i : dataset.getDatasets())
				if (i.first.substr(0, prefix.size()) == prefix)
					e->getDataset().getDatasets().insert(make_pair(i.first.substr(prefix.size()), i.second));
			assert(e->getData("item_name").getDataString() == name);
			assert(e->getData("count").getDataInt() == count);
			e->accelerateVector(1.0*rand01() + 0.1, 180.0 + 180.0*rand01()); // Give a random velocity
			entityManager.insert(e, centerPos);
			name.clear();
			count = 0;
		}
}

