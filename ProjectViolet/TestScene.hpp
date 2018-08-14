#pragma once

#include "Main.hpp"
#include "Scene.hpp"

#include "TextureManager.hpp"
#include "Chunk.hpp"
#include "AssetManager.hpp"
#include "TestEntity.hpp"
#include "PlayerEntity.hpp"
#include "TerrainGenerator.hpp"
#include "NovelGameSystem.hpp"

class TestScene :public Scene {
public:

	const string getSceneName() override {
		return "TestScene";
	}

	void preWindowInitalaize() override;
	void postWindowInitalaize(RenderWindow& win) override;

	void start(RenderWindow& win) override;

	void onRender(RenderWindow& win) override;

	void updateLogic(RenderWindow& win) override;

	void handleEvent(RenderWindow& win, Event& event) override;

	void runImGui() override;

	void stop() override;


private:

	bool _sendMousePressedToHandItem(Mouse::Button button);
	void _sendMouseReleasedToHandItem(Mouse::Button button);

	Uuid testEntity;

	bool showDebugInfo;
	bool showExtraImGuiWindows;

	Vector2i mouseWorldCoord;

	Texture background;

	TerrainGenerator prov;

	int terrainListSize, terrainMaskSize, particleListSize, entityListSize, totalVertSize;
};

