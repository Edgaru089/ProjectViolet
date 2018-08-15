#pragma once

#include "Entity.hpp"


class Mob :public Entity {
public:

	friend class WorldFileHandler;

	Mob() :Entity() {}
	virtual ~Mob() {}

	virtual const double getGravityAclc() override { return 8.0; }
	virtual const double getFrictionDeaclc() override { return 2.8; }

	virtual const int getMaxHealth() = 0;
	virtual const double getShieldFactor() { return 1.0; }

	void harm(int amount, Vector2d from, double knockbackFactor = 1.0) {
		harmUnprotected(amount*getShieldFactor(), from, knockbackFactor);
	}

	void harmUnprotected(int amount, Vector2d from, double knockbackFactor = 1.0) {
		// TODO Particles on harm
		// TODO Knockback on harm
		if (!alive)
			return;
		if (onGround)
			if (from.x > getPosition().x)
				accelerateVector(2.5 * knockbackFactor, 255);
			else
				accelerateVector(2.5 * knockbackFactor, 285);
		else
			if (from.x > getPosition().x)
				accelerateVector(2.5 * knockbackFactor, 220);
			else
				accelerateVector(2.5 * knockbackFactor, 320);
		if (health() <= amount) {
			health() = 0;
			kill();
		}
		else {
			health() -= amount;
		}
	}

	void kill() override;
	int getHealth() { return health(); }
	void setHealth(int health) { this->health() = health; }

protected:

	int& health() { return datasets["mob_health"].getDataInt(); };

};
