
#include "App.hpp"

#include "LogSystem.hpp"

#include "Scene.hpp"

#include "GameScene.hpp"
#include "TestScene.hpp"
#include "MapEditorScene.hpp"
#include "IO.hpp"

#include "TextSystem.hpp"


////////////////////////////////////////
void App::initalaizePreWindow() {

	Uuid::seed(random_device()());

	sceneMapper["GameScene"] = new GameScene();
	sceneMapper["TestScene"] = new TestScene();
	sceneMapper["MapEditorScene"] = new MapEditorScene();

	mlog << "[App] Initalaizing Asset Systems..." << dlog;
	assetManager.loadListFile();
	textureManager.bindTexture();

	for (pair<const string, Scene*>& i : sceneMapper)
		i.second->preWindowInitalaize();

	currentScene = sceneMapper["MapEditorScene"];

	text.loadFromFile("lang.list");

	hasLog = false;
}


////////////////////////////////////////
void App::initalaizePostWindow(RenderWindow& win) {
	for (pair<const string, Scene*>& i : sceneMapper)
		i.second->postWindowInitalaize(win);
}


////////////////////////////////////////
void App::start(RenderWindow& win) {
	currentScene->start(win);
	logicDeltaClock.restart();
}


////////////////////////////////////////
void App::onRender(RenderWindow& win) {

	currentScene->onRender(win);

}


////////////////////////////////////////
void App::runImGui() {

	//////////////////// Log Window ////////////////////
	if (hasLog) {
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		if (imgui::Begin("Logs", NULL, ImGuiWindowFlags_MenuBar)) {
			//////// Menu Bar ////////
			static bool follow = true;
			if (imgui::BeginMenuBar()) {
				if (imgui::BeginMenu("Controls")) {
					if (imgui::MenuItem("Clear"))
						dlog.clearBuffer();
					imgui::Separator();
					imgui::MenuItem("Follow the end of log     ", NULL, &follow);
					imgui::EndMenu();
				}
				imgui::EndMenuBar();
			}

			//////// Text Area ////////
			imgui::BeginChild("DigitLogScroll", Vector2i(0, 0), true);
			static float size;
			string total;
			for (const string& i : dlog.getBuffers())
				total += (i + '\n');
			imgui::TextUnformatted(total.c_str());
			if (size != imgui::GetScrollMaxY() && follow)
				imgui::SetScrollY(imgui::GetScrollMaxY());
			size = imgui::GetScrollMaxY();
			imgui::EndChild();
		}
		imgui::End();
	}

	currentScene->runImGui();
}


////////////////////////////////////////
void App::updateLogic(RenderWindow& win) {

	logicIO.deltaTime = logicDeltaClock.restart();
	logicIO.renderSize = Vector2i(win.getSize());
	logicIO.hasFocus = win.hasFocus();

	// Keystate and mouse position handled by game scene for pausing game issues
	if (currentScene->getSceneName() != "TestScene") {
		logicIO.mousePos = Mouse::getPosition(win);
		auto handleKeyState = [&](LogicIO::KeyState& state, bool isDown) {
			if (isDown) {
				if (state == LogicIO::Released || state == LogicIO::JustReleased)
					state = LogicIO::JustPressed;
				else
					state = LogicIO::Pressed;
			}
			else {
				if (state == LogicIO::Pressed || state == LogicIO::JustPressed)
					state = LogicIO::JustReleased;
				else
					state = LogicIO::Released;
			}
		};
		// Update key states
		// Keyboard
		for (int i = 0; i < Keyboard::KeyCount; i++)
			handleKeyState(logicIO.keyboardState[i], Keyboard::isKeyPressed((Keyboard::Key)i));
		// Mouse
		for (int i = 0; i < Mouse::ButtonCount; i++)
			handleKeyState(logicIO.mouseState[i], Mouse::isButtonPressed((Mouse::Button)i));
	}

	currentScene->updateLogic(win);

	Scene::updateScene(win);
}


////////////////////////////////////////
void App::handleEvent(RenderWindow& win, Event& e) {
	currentScene->handleEvent(win, e);
}


////////////////////////////////////////
void App::onViewportChange(RenderWindow& win) {

}


////////////////////////////////////////
void App::onStop() {
	currentScene->stop();
}


void App::switchScene(string name) {
	Scene::switchScene(sceneMapper[name]);
}

