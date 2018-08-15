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

	void clear() { entities.clear(); }

	Uuid insert(shared_ptr<Entity> entitiy, Vector2d position);

	// Does not call Entity::onCreate()
	void insert(Uuid id, shared_ptr<Entity> entity);

	shared_ptr<Entity> getEntity(Uuid id);

	unordered_map<Uuid, shared_ptr<Entity>, UuidHasher>& getEntityMapList() { return entities; }

public:

	// Summon an explosion
	void explode(Vector2d position, double force);

private:

	friend class WorldFileHandler;

	unordered_map<Uuid, shared_ptr<Entity>, UuidHasher> entities;

};


EntityManager entityManager;
