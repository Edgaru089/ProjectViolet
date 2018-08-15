#pragma once

#include "Main.hpp"
#include "MapEditorScene.hpp"
#include "WorldFileHandler.hpp"
#include "AssetManager.hpp"
#include "TextureManager.hpp"


////////////////////////////////////////
void MapEditorScene::preWindowInitalaize() {
	background.loadFromFile(assetManager.getAssetFilename("background_stone"));
	background.setRepeated(true);
}


////////////////////////////////////////
void MapEditorScene::start(RenderWindow& win) {
	renderIO.gameScaleFactor = 48.0;
	selectedEntities.clear();
	logicPaused = true;
}


////////////////////////////////////////
void MapEditorScene::onRender(RenderWindow& win) {
	renderIO.viewRect = FloatRect(-renderIO.gameScenePosOffset.x, -renderIO.gameScenePosOffset.y, logicIO.renderSize.x, logicIO.renderSize.y);
	renderIO.viewRectBlock = DoubleRect(
		renderIO.viewRect.left / renderIO.gameScaleFactor,
		renderIO.viewRect.top / renderIO.gameScaleFactor,
		renderIO.viewRect.width / renderIO.gameScaleFactor,
		renderIO.viewRect.height / renderIO.gameScaleFactor);

	VertexArray arr;
	if (renderTerrain) {
		// Background code borrowed from TestScene
		Sprite sp;
		sp.setTexture(background);
		sp.setScale(renderIO.gameScaleFactor / background.getSize().x * 1.5, renderIO.gameScaleFactor / background.getSize().y * 1.5);
		sp.setTextureRect(IntRect(0, 0, (win.getView().getSize().x + 2 * renderIO.gameScaleFactor * 1.5) / sp.getScale().x,
			(win.getView().getSize().y + 2 * renderIO.gameScaleFactor * 1.5) / sp.getScale().y));
		sp.setPosition(fmod(renderIO.gameScenePosOffset.x, renderIO.gameScaleFactor * 1.5) - renderIO.gameScaleFactor * 1.5,
					   fmod(renderIO.gameScenePosOffset.y, renderIO.gameScaleFactor * 1.5) - renderIO.gameScaleFactor * 1.5);
		win.draw(sp);
		terrainManager.getRenderList(arr);
	}
	terrainVertex = arr.getVertexCount();
	if (renderLightmask)
		terrainManager.getLightMask(arr);
	lightmaskVertex = arr.getVertexCount() - terrainVertex;
	if (renderEntity)
		entityManager.getRenderList(arr);
	entityVertex = arr.getVertexCount() - terrainVertex - lightmaskVertex;
	arr.setPrimitiveType(PrimitiveType::Triangles);

	win.setView(View(renderIO.viewRect));
	win.draw(arr, textureManager.getBindingTexture());

	if (renderEntity)
		for (auto i : entityManager.getEntityMapList())
			if (i.second != nullptr)
				if (inspectingEntities.find(i.first) != inspectingEntities.end())
					win.draw(renderRect(i.second->getHitbox()*renderIO.gameScaleFactor, Color(255, 32, 32)));
				else if (selectedEntities.find(i.first) != selectedEntities.end())
					win.draw(renderRect(i.second->getHitbox()*renderIO.gameScaleFactor));
				else
					win.draw(renderRect(i.second->getHitbox()*renderIO.gameScaleFactor, Color(255, 255, 255, 160)));

	win.setView(View(FloatRect(Vector2f(0, 0), Vector2f(logicIO.renderSize))));
	if (!imgui::GetIO().WantCaptureMouse&&logicIO.mouseState[Mouse::Left] == LogicIO::Pressed)
		win.draw(renderRect(DoubleRect(min(mousePosBeforePress.x, Mouse::getPosition(win).x), min(mousePosBeforePress.y, Mouse::getPosition(win).y),
									   abs(mousePosBeforePress.x - Mouse::getPosition(win).x), abs(mousePosBeforePress.y - Mouse::getPosition(win).y)),
							Color(255, 255, 255, 192)));
}


////////////////////////////////////////
void MapEditorScene::updateLogic(RenderWindow& win) {

	if (!imgui::GetIO().WantCaptureMouse&&logicIO.mouseState[Mouse::Left] == LogicIO::JustPressed)
		mousePosBeforePress = Mouse::getPosition(win);
	if (!imgui::GetIO().WantCaptureMouse&&logicIO.mouseState[Mouse::Left] == LogicIO::JustReleased) {
		Vector2d lastpos = TerrainManager::convertScreenPixelToWorldCoord(mousePosBeforePress),
			curpos = TerrainManager::convertScreenPixelToWorldCoord(Mouse::getPosition(win));
		DoubleRect rect(min(lastpos.x, curpos.x), min(lastpos.y, curpos.y), abs(lastpos.x - curpos.x), abs(lastpos.y - curpos.y));
		selectedEntities.clear();
		for (auto i : entityManager.getEntityMapList())
			if (i.second != nullptr && (i.second->getHitbox().intersects(rect) || i.second->getHitbox().contains(curpos)))
				selectedEntities.insert(i.first);
	}
	if (!imgui::GetIO().WantCaptureMouse&&logicIO.mouseState[Mouse::Middle] == LogicIO::JustPressed)
		mousePosBeforePress = Mouse::getPosition(win);
	if (!imgui::GetIO().WantCaptureMouse&&logicIO.mouseState[Mouse::Middle] == LogicIO::Pressed) {
		renderIO.gameScenePosOffset += Vector2d(Mouse::getPosition(win) - mousePosBeforePress);
		mousePosBeforePress = Mouse::getPosition(win);
	}

	if (!imgui::GetIO().WantCaptureMouse&&logicIO.mouseState[Mouse::Right] == LogicIO::JustPressed)
		mousePosBeforePress = logicIO.mousePos;
	if (!imgui::GetIO().WantCaptureMouse&&logicIO.mouseState[Mouse::Right] == LogicIO::Pressed) {
		bool moving = false;
		for (auto& i : inspectingEntities) {
			auto e = entityManager.getEntity(i);
			if (e != nullptr&&e->getHitbox().contains(TerrainManager::convertScreenPixelToWorldCoord(logicIO.mousePos)))
				e->setPosition(e->getPosition() + Vector2d(Mouse::getPosition(win) - mousePosBeforePress));
		}
		mousePosBeforePress = logicIO.mousePos;
	}

	if (!logicPaused) {
		terrainManager.updateLogic();
		entityManager.updateLogic();
	}
}


////////////////////////////////////////
void MapEditorScene::runImGui() {
	float menuBarHeight = .0f;
	if (imgui::BeginMainMenuBar()) {
		menuBarHeight = imgui::GetWindowSize().y;

		imgui::EndMainMenuBar();
	}

	imgui::SetNextWindowPos(ImVec2(8.0f, 8.0f + menuBarHeight), ImGuiCond_Always);
	if (imgui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove)) {
		if (imgui::BeginMenu("Load File...                  ")) {
			imgui::Text("Filename: ");
			static char buf[128];
			imgui::PushItemWidth(250.0f);
			if (imgui::InputText("##LoadFilename", buf, 128, ImGuiInputTextFlags_EnterReturnsTrue))
				WorldFileHandler::loadFromFile(buf, terrainManager, entityManager);
			imgui::PopItemWidth();
			imgui::EndMenu();
		}
		if (imgui::BeginMenu("Save File...")) {
			imgui::Text("Filename: ");
			static char buf[128];
			imgui::PushItemWidth(250.0f);
			if (imgui::InputText("##SaveFilename", buf, 128, ImGuiInputTextFlags_EnterReturnsTrue))
				WorldFileHandler::saveToFile(buf, terrainManager, entityManager);
			imgui::PopItemWidth();
			imgui::EndMenu();
		}
		imgui::Separator();
		if (logicPaused) {
			imgui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
			if (imgui::Selectable("Logic:", false))
				logicPaused = !logicPaused;
			imgui::PopItemFlag();
			imgui::SameLine();
			imgui::PushStyleColor(ImGuiCol_Text, Color::Red);
			imgui::Text("PAUSED");
			imgui::PopStyleColor();
		}
		else {
			imgui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
			if (imgui::Selectable("Logic:", false))
				logicPaused = !logicPaused;
			imgui::PopItemFlag();
			imgui::SameLine();
			imgui::PushStyleColor(ImGuiCol_Text, Color::Green);
			imgui::Text("RUNNING");
			imgui::PopStyleColor();
		}
		imgui::Separator();
		static float dummy = 48.0f;
		imgui::PushItemWidth(200);
		imgui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
		imgui::MenuItem("Render Terrain", nullptr, &renderTerrain);
		imgui::MenuItem("Render Terrain Lightmask", nullptr, &renderLightmask);
		imgui::MenuItem("Render Entities", nullptr, &renderEntity);
		imgui::MenuItem("Use Fancy Lightmask", nullptr, &renderIO.useFancyLightmask);
		imgui::PopItemFlag();
		imgui::PopItemWidth();
		imgui::Separator();
		imgui::SliderFloat("Render Size", &dummy, 16.0f, 96.0f);
		renderIO.gameScaleFactor = dummy;
		imgui::Separator();
		imgui::Text("Terrain:   %d verts (%d tri)",
					terrainVertex, terrainVertex / 3);
		imgui::Text("Lightmask: %d verts (%d tri)",
					lightmaskVertex, lightmaskVertex / 3);
		imgui::Text("Entity:    %d verts (%d tri)",
					entityVertex, entityVertex / 3);
		imgui::Text("Total:     %d verts (%d tri)",
					terrainVertex + lightmaskVertex + entityVertex, (terrainVertex + lightmaskVertex + entityVertex) / 3);
		imgui::Separator();
		imgui::Text("Terrain Chunk Count: %dx%d, Chunk Size: %dx%d",
					terrainManager.getChunkCount().x, terrainManager.getChunkCount().y, chunkSize, chunkSize);
		imgui::Text("Entity Count: %d", entityManager.getEntityMapList().size());
		imgui::Separator();
		if (imgui::Button("Jump to TestScene", ImVec2(-1, 0)))
			app->switchScene("TestScene");
	}
	imgui::End();

	auto pushBoolText = [](bool value, bool callSameLine = true) {
		if (callSameLine)
			imgui::SameLine();
		if (value)
			imgui::PushStyleColor(ImGuiCol_Text, Color::Green);
		else
			imgui::PushStyleColor(ImGuiCol_Text, Color::Red);
		imgui::Text("%s", value ? "True" : "False");
		imgui::PopStyleColor();
	};

	inspectingEntities.clear();
	auto pushEntityTreeNode = [&](Uuid id, bool defaultOpen = false) {
		auto e = entityManager.getEntity(id);
		if (e == nullptr)
			return;
		if (imgui::TreeNodeEx((e->getEntityId() + ", {" + e->getUuid().toString() + "}").c_str(), defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0)) {
			if (inspectingEntities.find(id) == inspectingEntities.end())
				inspectingEntities.insert(id);
			imgui::Text("Id: %s, Uuid: {%s}", e->getEntityId().c_str(), id.toString().c_str());
			float pos[2] = { e->getPosition().x, e->getPosition().y };
			float vec[2] = { e->getVelocity().x, e->getVelocity().y };
			imgui::InputFloat2("Position", pos, 8);
			imgui::InputFloat2("Velocity", vec, 8);
			e->setPosition(Vector2d(pos[0], pos[1]));
			e->setVelocity(Vector2d(vec[0], vec[1]));
			if (imgui::TreeNode("Datasets                               ")) {
				if (imgui::BeginMenu("Add Data...")) {
					static Data d;
					static int type = 0;
					imgui::Combo("Data Type", &type, "Integer\0String\0Bool\0\0");
					static char namebuf[64];
					imgui::InputText("Data Id String", namebuf, 64);
					d.setType((Data::DataType)type);
					if (type == Data::Integer)
						imgui::InputInt("Integer##InputDataInt", &d.getDataInt());
					else if (type == Data::String) {
						static char buf[128];
						strcpy(buf, d.getDataString().c_str());
						imgui::InputText("String##InputDataString", buf, 128);
					}
					else if (type == Data::Bool) {
						imgui::Checkbox("Bool##InputDataBool", &d.getDataBool());
						imgui::SameLine();
						pushBoolText(d.getDataBool(), false);
					}
					if (imgui::Button("Add!", ImVec2(-1, 0))) {
						e->getDataset().getDatasets().insert(make_pair(namebuf, d));
						namebuf[0] = '\0';
						d = Data();
						type = 0;
					}
					imgui::EndMenu();
				}
				if (e->getDataset().getDatasets().size() == 0)
					imgui::Text("None!");
				for (auto j = e->getDataset().getDatasets().begin(); j != e->getDataset().getDatasets().end();) {
					auto& i = *j;
					static float buttonHeight = .0f;
					if (i.second.getType() == Data::Integer)
						imgui::InputInt(i.first.c_str(), &i.second.getDataInt());
					else if (i.second.getType() == Data::String) {
						static char buf[128];
						strcpy(buf, i.second.getDataString().c_str());
						imgui::InputText(i.first.c_str(), buf, 128);
					}
					else if (i.second.getType() == Data::Bool) {
						imgui::Selectable(i.first.c_str(), &i.second.getDataBool(), 0, ImVec2(0, buttonHeight));
						imgui::SameLine();
						pushBoolText(i.second.getDataBool(), false);
					}
					else if (i.second.getType() == Data::Uuid)
						imgui::Text("{%s} :%s", i.second.getDataUuid().toString(), i.first.c_str());
					imgui::SameLine(23.0f);
					if (imgui::Button(("Remove##Remove" + i.first).c_str()))
						j = e->getDataset().getDatasets().erase(j);
					else
						j++;
					buttonHeight = imgui::GetItemRectSize().y;
				}
				imgui::TreePop();
			}
			imgui::TreePop();
		}
		else {
			auto i = inspectingEntities.find(id);
			if (i != inspectingEntities.end())
				inspectingEntities.erase(i);
		}
	};
	if (imgui::Begin("Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		if (imgui::TreeNodeEx("Selected Entites", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed)) {
			if (selectedEntities.empty())
				imgui::Text("Nothing selected.");
			else {
				for (auto& i : selectedEntities)
					pushEntityTreeNode(i, true);
			}
			imgui::TreePop();
		}
		if (imgui::TreeNodeEx("All Entites", ImGuiTreeNodeFlags_Framed)) {
			for (auto& i : entityManager.getEntityMapList())
				pushEntityTreeNode(i.first);
			imgui::TreePop();
		}
	}
	imgui::End();

	if (imgui::Begin("Insert Or Assign")) {

	}
	imgui::End();
}


////////////////////////////////////////
void MapEditorScene::stop() {

}

