#pragma once

#include "Entity.hpp"
#include "IO.hpp"
#include "Uuid.hpp"


class EntityManager : public Lockable {
public:

	friend class Entity;

	void initalaize();

	void updateLogic();

	// Triangles
	void getRenderList(VertexArray& verts);

public:

	Uuid insert(shared_ptr<Entity> entitiy, Vector2d position);

	shared_ptr<Entity> getEntity(Uuid id);

	map<Uuid, shared_ptr<Entity>>& getEntityMapList() { return entities; }

public:

	// Summon an explosion
	void explode(Vector2d position, double force, bool damageTerrain = gameIO.ruleExplosionDamagesTerrain);

private:

	map<Uuid, shared_ptr<Entity>> entities;

};


EntityManager entityManager;
