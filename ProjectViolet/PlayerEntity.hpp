#pragma once

#include "Main.hpp"
#include "Entity.hpp"
#include "Mob.hpp"
#include "InventoryObject.hpp"


class ItemEntity;


class PlayerEntity :public Mob, public InventoryObject {
public:

	PlayerEntity();

	const string getEntityId() override { return "player"; }
	const double getFrictionDeaclc() override { return 45.0; }
	const double getGravityAclc() override {
		if (!onLadder())
			return 15.0;
		else
			return 0.0;
	}

	const int getMaxHealth() override { return 100; }

	const double getMaxSpeed() {
		if (!crouched())
			return 7.5;
		else
			return 3.0;
	}

	TextureInfo getTextureInfo() override {
		if (!crouched())
			return textureManager.getTextureInfo("entity_" + getEntityId());
		else
			return textureManager.getTextureInfo("entity_" + getEntityId() + "_crouched");
	}

	Vector2d getSize() override {
		if (!crouched())
			return Vector2d(0.8, 1.6);
		else
			return Vector2d(0.8, 0.8);
	}

	Vector2d getEyePosition() {
		return getPosition() + Vector2d(0, -getSize().y + 0.3);
	}

	const Vector2i getInventorySize() override { return Vector2i(4, 9); }

	string getPlayerName() { return name(); };
	string setPlayerName(string name) { this->name() = name; }

	bool isLocalPlayer() { return islocal; }
	void setIsLocalPlayer(bool set) { islocal = set; }

	const bool requestSpeicalRendering() override { return true; }
	void _pushTriangleVertexes(VertexArray& arr) override;

	void _updateLogic() override;

public:

	void jump();

	void moveLeft(bool state = true);
	void moveRight(bool state = true);

	bool isCrouched() { return crouched(); }
	bool crouch(bool state = true);
	bool isStuck() { return wantStandup(); }

	bool isOnLadder() { return onLadder(); }
	void ascendLadder(bool state = true);
	void decendLadder(bool state = true);
	void jumpOffLadder();

	bool collectItem(ItemEntity& item);

private:

	string & name() { return datasets["name"].getDataString(); }

	bool& isMovingLeft() { return datasets["is_moving_left"].getDataBool(); }
	bool& isMovingRight() { return datasets["is_moving_right"].getDataBool(); }
	bool& crouched() { return datasets["crouched"].getDataBool(); }
	bool& wantStandup() { return datasets["want_standup"].getDataBool(); }
	bool& onLadder() { return datasets["on_ladder"].getDataBool(); }
	bool& isAscendingLadder() { return datasets["ladder_up"].getDataBool(); }
	bool& isDecendingLadder() { return datasets["ladder_down"].getDataBool(); }

	bool islocal;

};


shared_ptr<PlayerEntity> localPlayer;

