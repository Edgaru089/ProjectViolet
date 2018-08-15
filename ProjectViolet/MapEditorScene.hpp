#pragma once

#include "Scene.hpp"
#include "TerrainManager.hpp"
#include "EntityManager.hpp"


class MapEditorScene :public Scene {
public:

	const string getSceneName() override { return "MapEditorScene"; }

	void preWindowInitalaize() override;

	void start(RenderWindow& win) override;

	void onRender(RenderWindow& win) override;

	void updateLogic(RenderWindow& win) override;

	void runImGui() override;

	void stop() override;

private:
	Texture background;

	int terrainVertex, lightmaskVertex, entityVertex;
	bool renderTerrain, renderLightmask, renderEntity;

	bool logicPaused;

	Vector2i mousePosBeforePress;

	unordered_set<Uuid, UuidHasher> selectedEntities;
	unordered_set<Uuid, UuidHasher> inspectingEntities;

	
};


