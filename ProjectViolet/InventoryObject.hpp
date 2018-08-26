#pragma once

#include "Main.hpp"

class InventoryObject {
public:

	virtual const Vector2i getInventorySize() = 0;

	void dropAllItems(Dataset& dataset, Vector2d centerPos);

};

