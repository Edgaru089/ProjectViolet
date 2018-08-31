#pragma once

#include "Scene.hpp"
#include "TerrainManager.hpp"
#include "EntityManager.hpp"
#include "MobPathFinder.hpp"


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

	set<Uuid> selectedEntities;
	set<Uuid> inspectingEntities;
	Vector2i selectedBlock;

	bool insertingEntity, insertingBlock;
	string insertId;

	Vector2i findPathBeginPos = Vector2i(-1, -1), findPathEndPos = Vector2i(-1, -1);
	MobPath path;
};


