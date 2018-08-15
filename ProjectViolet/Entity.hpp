#pragma once

#include "Main.hpp"
#include "Data.hpp"
#include "Chunk.hpp"
#include "TextureManager.hpp"
#include "TerrainManager.hpp"
#include "Block.hpp"
#include "Uuid.hpp"


class Entity : public Lockable {
public:

	friend class WorldFileHandler;

	Entity() :alive(false), uuid(Uuid::nil()), posX(0.0), posY(0.0), vecX(0.0), vecY(0.0) {}
	virtual ~Entity() {}

	virtual const double getFrictionDeaclc() { return 0.5; }
	virtual const double getGravityAclc() { return 2.8; }

	virtual const string getEntityId() = 0;

	virtual const bool requestEntityCollisionCallback() { return false; }
	virtual void _onCollideEntity(shared_ptr<Entity> collided) {}


	void updateLogic();

	void onCreate() { alive = true; _onCreate(); }


	virtual TextureInfo getTextureInfo() { return textureManager.getTextureInfo("entity_" + getEntityId()); }

	virtual const bool requestSpeicalRendering() { return false; }
	virtual void _pushTriangleVertexes(VertexArray& arr) {}


	void setPosition(Vector2d pos) { posX = pos.x; posY = pos.y; }
	Vector2d getPosition() { return Vector2d(posX, posY); }
	Vector2d getCenterPos() { return getPosition() + Vector2d(.0, -getSize().y / 2.0); }

	// Velocity: blocks per second
	void setVelocity(Vector2d vec) { vecX = vec.x; vecY = vec.y; }
	Vector2d getVelocity() { return Vector2d(vecX, vecY); }
	void accelerate(Vector2d acc) { vecX += acc.x; vecY += acc.y; }

	// Deg: Counter-Clockwise, from x-axis (right)
	void setVelocityVector(double speed, double deg) {
		vecX = cos(deg*PI / 180.0)*speed;
		vecY = sin(deg*PI / 180.0)*speed;
	}

	void accelerateVector(double speed, double deg) {
		vecX += cos(deg*PI / 180.0)*speed;
		vecY += sin(deg*PI / 180.0)*speed;
	}

	const bool isAlive() { return alive; }
	virtual void kill() { alive = false; _onKill(); }
	double getRotation() { return angle; }

	virtual Vector2d getSize() { return Vector2d(0.2, 0.2); }
	virtual DoubleRect getHitbox() {
		return DoubleRect(
			getPosition().x - getSize().x / 2.0,
			getPosition().y - getSize().y,
			getSize().x,
			getSize().y);
	}

	bool isOnGround() { return onGround; }

	Uuid getUuid() { return uuid; }
	void setUuid(Uuid id) { uuid = id; }

	Data& getData(string id) {
		return datasets[id];
	}

	template<typename Type>
	void setData(string id, Type data) {
		datasets[id].setData(data);
	}

	Dataset& getDataset() { return datasets; }

	virtual void _onCreate() {}
	virtual void _updateLogic() {}
	virtual void _onKill() {}

	enum CollisionBoxEdge {
		Left,
		Top,
		Right,
		Bottom
	};

	virtual void _onCollision(shared_ptr<Block> block, CollisionBoxEdge whichEntityEdge) {}

protected:

	virtual void _getBoundingCollisionPoints(vector<Vector2d>& vec);

private:

	void _moveX(double);
	void _moveY(double);

protected:

	double posX, posY;
	double vecX, vecY;
	double angle;
	bool onGround;

	Uuid uuid;

	bool alive;

	Dataset datasets;

};

