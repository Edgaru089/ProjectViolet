#pragma once

#include "Entity.hpp"
#include "PlayerEntity.hpp"

const int maxArrowDamage = 10;

class ArrowEntity :public Entity {
public:

	static void shoot(double damage = maxArrowDamage,
					  Vector2d position = localPlayer->getEyePosition(),
					  double degree = gameIO.degreeAngle);

public:

	ArrowEntity() { inWall() = false; }

	const string getEntityId() override { return "arrow"; }
	const bool requestSpeicalRendering() override { return true; }
	void _pushTriangleVertexes(VertexArray& verts) override;

	const bool requestEntityCollisionCallback() override { return !(inWall() || inEntity()); }
	void _onCollideEntity(shared_ptr<Entity> e) override;

	const double getGravityAclc() override { return 2.0; }

	Vector2d getSize() override { return Vector2d(0.2, 0.2); }

	void _updateLogic() override;
	void _onCollision(shared_ptr<Block> block, CollisionBoxEdge) override;

private:

	void _dropItem();

	Clock particleClock;

	double damage;

	bool& inWall() { return getData("in_wall").getDataBool(); };
	bool& inEntity() { return getData("in_entity").getDataBool(); }

	Vector2d pos0;
	Vector2i inWallBlock;
	Uuid inEntityId;
	double angle0;

};

