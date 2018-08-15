#pragma once

#include "Main.hpp"

USING_NAMESPACE;

class Scene {
public:

	virtual const string getSceneName() = 0;

	virtual void preWindowInitalaize() {}
	virtual void postWindowInitalaize(RenderWindow& win) {}

	virtual void start(RenderWindow& win) = 0;

	virtual void onRender(RenderWindow& win) = 0;

	virtual void updateLogic(RenderWindow& win) = 0;

	virtual void handleEvent(RenderWindow& win, Event& event) {}

	virtual void runImGui() {}

	virtual void stop() = 0;

public:

	static void switchScene(Scene* newScene);
	static void updateScene(RenderWindow& win);

};


// Map all pointers to scenes to this std::map
unordered_map<string, Scene*> sceneMapper;
Scene* currentScene, *switchingScene;


void Scene::switchScene(Scene* newScene) {
	switchingScene = newScene;
}

void Scene::updateScene(RenderWindow& win) {
	if (switchingScene != nullptr) {
		currentScene->stop();
		switchingScene->start(win);
		currentScene = switchingScene;
		switchingScene = nullptr;
	}
}
