#pragma once

#include "Entity.hpp"
#include "IO.hpp"
#include "PlayerEntity.hpp"

const double grenadeDamage = 16.0;

class GrenadeEntity :public Entity {
public:

	static void throwGrenade(Time triggerTime = seconds(2.5), double degree = gameIO.degreeAngle, Vector2d pos = localPlayer->getEyePosition());

public:

	const string getEntityId() override { return "grenade"; }
	const double getGravityAclc() override { return 4.8; }
	TextureInfo getTextureInfo() override { return textureManager.getTextureInfo("entity_grenade"); }

	Vector2d getSize() override { return Vector2d(0.4, 0.4); }

	void _updateLogic() override;
	void _onCollision(shared_ptr<Block> b, CollisionBoxEdge whichEntityEdge) override;

private:
	Time triggerTime;
	int& fuseBurnedTimeMill() { return datasets["fused_time"].getDataInt(); }

	bool wantSetSpeed;
	Vector2d setSpeed;
};
