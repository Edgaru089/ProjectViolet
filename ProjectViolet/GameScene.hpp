#pragma once

#include "Main.hpp"
#include "Scene.hpp"


class GameScene :public Scene {
public:
	const string getSceneName() override { return "GameScene"; }

	void preWindowInitalaize() override;

	
	void start(RenderWindow& win) override;


	void onRender(RenderWindow& win) override;


	void updateLogic(RenderWindow& win) override;

	
	void stop() override;

private:




};

