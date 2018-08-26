#pragma once

#include "Main.hpp"
#include "MapEditorScene.hpp"
#include "WorldFileHandler.hpp"
#include "AssetManager.hpp"
#include "TextureManager.hpp"


////////////////////////////////////////
void MapEditorScene::preWindowInitalaize() {
	assetManager.getAssetData("background_stone").load(background);
	background.setRepeated(true);
}


////////////////////////////////////////
void MapEditorScene::start(RenderWindow& win) {
	renderIO.gameScaleFactor = 48.0;
	selectedEntities.clear();
	selectedBlock = Vector2i(-1, -1);
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

	if (selectedBlock != Vector2i(-1, -1)) {
		auto b = terrainManager.getBlock(selectedBlock);
		if (b == nullptr) {
			Vector2d center = Vector2d(selectedBlock.x + 0.5, selectedBlock.y + 0.5)*renderIO.gameScaleFactor;
			double width = renderIO.gameScaleFactor;
			VertexArray verts;
			verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y - width / 2.0), Color(255, 255, 255, 192)));
			verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - width / 2.0), Color(255, 255, 255, 192)));
			verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y + width / 2.0), Color(255, 255, 255, 192)));
			verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y + width / 2.0), Color(255, 255, 255, 192)));
			verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y - width / 2.0), Color(255, 255, 255, 192)));
			verts.setPrimitiveType(PrimitiveType::LinesStrip);
			win.draw(verts);
		}
		else
			win.draw(renderRect(b->getHitbox()*renderIO.gameScaleFactor));
	}

	Vector2i mousePos = TerrainManager::convertScreenPixelToWorldBlockCoord(logicIO.mousePos);
	shared_ptr<Block> b = terrainManager.getBlock(mousePos);
	if (b == nullptr) {
		Vector2d center = Vector2d(mousePos.x + 0.5, mousePos.y + 0.5)*renderIO.gameScaleFactor;
		double width = renderIO.gameScaleFactor;
		VertexArray verts;
		verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y - width / 2.0), Color(255, 255, 255, 128)));
		verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - width / 2.0), Color(255, 255, 255, 128)));
		verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y + width / 2.0), Color(255, 255, 255, 128)));
		verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y + width / 2.0), Color(255, 255, 255, 128)));
		verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y - width / 2.0), Color(255, 255, 255, 128)));
		verts.setPrimitiveType(PrimitiveType::LinesStrip);
		win.draw(verts);
	}
	else
		win.draw(renderRect(b->getHitbox()*renderIO.gameScaleFactor, Color(255, 255, 255, 128)));

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
		if (selectedEntities.size() == 0)
			if (selectedBlock == Vector2i(-1, -1))
				selectedBlock = TerrainManager::convertScreenPixelToWorldBlockCoord(logicIO.mousePos);
			else
				selectedBlock = Vector2i(-1, -1);
	}
	if (!imgui::GetIO().WantCaptureMouse&&logicIO.mouseState[Mouse::Middle] == LogicIO::JustPressed)
		mousePosBeforePress = Mouse::getPosition(win);
	if (!imgui::GetIO().WantCaptureMouse&&logicIO.mouseState[Mouse::Middle] == LogicIO::Pressed) {
		renderIO.gameScenePosOffset += Vector2d(Mouse::getPosition(win) - mousePosBeforePress);
		mousePosBeforePress = Mouse::getPosition(win);
	}

	//if (!imgui::GetIO().WantCaptureMouse&&logicIO.mouseState[Mouse::Right] == LogicIO::JustPressed)
	//	mousePosBeforePress = logicIO.mousePos;
	//if (!imgui::GetIO().WantCaptureMouse&&logicIO.mouseState[Mouse::Right] == LogicIO::Pressed) {
	//	bool moving = false;
	//	for (auto& i : selectedEntities) {
	//		auto e = entityManager.getEntity(i);
	//		if (e != nullptr&&e->getHitbox().contains(TerrainManager::convertScreenPixelToWorldCoord(logicIO.mousePos))) {
	//			Vector2d move = (Vector2d(Mouse::getPosition(win) - mousePosBeforePress)) / renderIO.gameScaleFactor;
	//			e->_moveX(move.x);
	//			e->_moveY(move.y);
	//		}
	//	}
	//	mousePosBeforePress = logicIO.mousePos;
	//}

	if (!imgui::GetIO().WantCaptureMouse&&logicIO.mouseState[Mouse::Right] == LogicIO::JustReleased) {
		if (insertingEntity) {
			auto p = entityAllocator.allocate(insertId);
			if (p != nullptr) {
				Vector2d pos = TerrainManager::convertScreenPixelToWorldCoord(logicIO.mousePos);
				entityManager.insert(p, pos);
			}
		}
		else if (insertingBlock) {
			Vector2i pos = TerrainManager::convertScreenPixelToWorldBlockCoord(logicIO.mousePos);
			terrainManager.setBlock(pos, insertId);
		}
	}

	if (!imgui::GetIO().WantCaptureKeyboard&&logicIO.keyboardState[Keyboard::Delete] == LogicIO::JustReleased) {
		for (auto& i : selectedEntities)
			entityManager.getEntityMapList().erase(i);
		selectedEntities.clear();
	}

	if (!logicPaused) {
		terrainManager.updateLogic();
		entityManager.updateLogic();
	}
}


////////////////////////////////////////
void MapEditorScene::runImGui() {
	auto pushBoolText = [](bool value, string trueText = "True", string falseText = "False", bool callSameLine = true) {
		if (callSameLine)
			imgui::SameLine();
		if (value)
			imgui::PushStyleColor(ImGuiCol_Text, Color::Green);
		else
			imgui::PushStyleColor(ImGuiCol_Text, Color::Red);
		imgui::Text("%s", value ? trueText.c_str() : falseText.c_str());
		imgui::PopStyleColor();
	};

	float menuBarHeight = .0f;
	if (imgui::BeginMainMenuBar()) {
		menuBarHeight = imgui::GetWindowSize().y;

		imgui::EndMainMenuBar();
	}

	imgui::SetNextWindowPos(ImVec2(8.0f, 8.0f + menuBarHeight), ImGuiCond_Always);
	if (imgui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove)) {
		if (imgui::BeginMenu("New A Map...")) {
			static int sizes[2];
			static char blockName[128] = { "stone" };
			imgui::InputInt2("Size (Chunks, WidthxHeight)", sizes);
			imgui::InputText("Fill Block Id", blockName, 128);
			pushBoolText(blockAllocator.allocs.find(blockName) != blockAllocator.allocs.end(),
						 "Valid", "Nullptr");
			if (imgui::Button("Create!", ImVec2(-1, 0))) {
				terrainManager.chunks.clear();
				terrainManager.chunkCount = Vector2i(sizes[0], sizes[1]);
				for (int i = 0; i < sizes[0]; i++)
					for (int j = 0; j < sizes[1]; j++)
						terrainManager.loadEmptyChunk(Vector2i(i, j), blockName);
			}
			imgui::EndMenu();
		}
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
		imgui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
		if (imgui::Selectable("Logic:", false))
			logicPaused = !logicPaused;
		imgui::PopItemFlag();
		pushBoolText(!logicPaused, "RUNNING", "PAUSED");
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
						pushBoolText(d.getDataBool());
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
						i.second.getDataString() = buf;
					}
					else if (i.second.getType() == Data::Bool) {
						imgui::Selectable(i.first.c_str(), &i.second.getDataBool(), 0, ImVec2(0, buttonHeight));
						pushBoolText(i.second.getDataBool());
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

	if (imgui::Begin("Insert", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		if (imgui::TreeNodeEx("Insert Entitiy", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed)) {
			static char entityName[128];
			imgui::InputText("Entity Id", entityName, 128);
			pushBoolText(entityAllocator.allocs.find(entityName) != entityAllocator.allocs.end(),
						 "Valid", "Invalid");
			imgui::Selectable("Inserting Entity", &insertingEntity);
			if (insertingEntity) {
				insertingBlock = false;
				insertId = entityName;
			}
			pushBoolText(insertingEntity, "ON", "OFF");
			imgui::TreePop();
		}
		if (imgui::TreeNodeEx("Set Block", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed)) {
			static char blockName[128];
			imgui::InputText("Block Id", blockName, 128);
			pushBoolText(blockAllocator.allocs.find(blockName) != blockAllocator.allocs.end(),
						 "Valid", "Nullptr");
			imgui::Selectable("Setting Block", &insertingBlock);
			if (insertingBlock) {
				insertingEntity = false;
				insertId = blockName;
			}
			pushBoolText(insertingBlock, "ON", "OFF");
			if (imgui::Button("Update Lighting", ImVec2(-1, 0)))
				terrainManager._updateLighting();
			imgui::TreePop();
		}
	}
	imgui::End();

	if (imgui::Begin("Inventory Manager", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		static int which = 0;
		static int invSize[2] = { -1, -1 };
		imgui::RadioButton("Entity", &which, 0);
		imgui::SameLine(.0f, 10.0f);
		imgui::RadioButton("Block", &which, 1);
		imgui::InputInt2("Inventory Size (LxC)", invSize);
		Dataset* data = nullptr;
		if (which == 0) { // Entity
			Uuid id;
			if (selectedEntities.size() == 1)
				id = *selectedEntities.begin();
			imgui::Text("Selected Entity: %s", selectedEntities.size() == 0 ? "None!" : (selectedEntities.size() > 1 ? "Too Many!" : ('{' + id.toString() + '}').c_str()));
			if (id != Uuid::nil()) {
				auto e = entityManager.getEntity(id);
				try {
					auto& inv = dynamic_cast<InventoryObject&>(*e);
					invSize[0] = inv.getInventorySize().x;
					invSize[1] = inv.getInventorySize().y;
				}
				catch (bad_cast) {}
				data = &e->getDataset();
			}
		}
		else if (which == 1) { // Block
			if (selectedBlock == Vector2i(-1, -1))
				imgui::Text("Selected Block: None!");
			else {
				auto b = terrainManager.getBlock(selectedBlock);
				imgui::Text("Selected Block: (%d, %d)", selectedBlock.x, selectedBlock.y);
				pushBoolText(b != nullptr, "Valid", "Invalid");
				if (b != nullptr) {
					try {
						auto& inv = dynamic_cast<InventoryObject&>(*b);
						invSize[0] = inv.getInventorySize().x;
						invSize[1] = inv.getInventorySize().y;
					}
					catch (bad_cast) {}
					data = &b->getDataset();
				}
			}
		}
		if (data != nullptr) {
			// borrow code from InGameUI
			imgui::PushStyleColor(ImGuiCol_ModalWindowDarkening, Color(0, 0, 0, 128));
			imgui::PushStyleColor(ImGuiCol_Button, Color::Transparent);
			imgui::PushStyleColor(ImGuiCol_ButtonHovered, Color(255, 255, 255, 64));
			imgui::PushStyleColor(ImGuiCol_ButtonActive, Color(255, 255, 255, 48));
			imgui::PushStyleColor(ImGuiCol_FrameBg, Color(0, 0, 0, 128));
			imgui::PushStyleColor(ImGuiCol_PopupBg, Color(0, 0, 0, 128));
			imgui::PushStyleColor(ImGuiCol_TitleBgActive, imgui::GetStyleColorVec4(ImGuiCol_PopupBg));
			imgui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
			imgui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			imgui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
			Dataset& dataset = *data;
			static bool contextMenuOpen = false;
			bool thisFrameHasContextMenu = false;
			for (int i = 0; i < invSize[0]; i++) {
				for (int j = 0; j < invSize[1]; j++) {
					if (dataset.getDatasets().find(to_string(i) + to_string(j) + "item_name") == dataset.getDatasets().end())
						continue;

					string slotPrefix = to_string(i) + to_string(j);
					string slotName = dataset[to_string(i) + to_string(j) + "item_name"].getDataString();
					int slotCount = dataset[to_string(i) + to_string(j) + "count"].getDataInt();

					TextureInfo info = textureManager.getTextureInfo(dataset[to_string(i) + to_string(j) + "item_name"].getDataString());
					if (!info.vaild)
						info = textureManager.getTextureInfo("none");
					shared_ptr<Item> item = nullptr;
					if (slotName.size() > 5)
						item = itemAllocator.allocate(slotName.substr(5), dataset, to_string(i) + to_string(j), false);

					int maxItemsThisSlot = maxItemsPerSlot;
					if (item != nullptr)
						maxItemsThisSlot = item->getMaxItemsPerSlotCount();

					if (j != 0)
						imgui::SameLine();
					imgui::PushID(i * 9 + j);
					bool mapMutated = false;
					imgui::ImageButton(info.getSprite(), Vector2f(32, 32), 3);

					if (info.id != "none"&&slotCount != 1) {
						ImVec2 pos = imgui::GetItemRectMin(); pos.x += 2.0;
						imgui::GetCurrentWindow()->DrawList->AddText(pos, ImU32(0xFFFFFFFF), StringParser::toString(slotCount).c_str());
					}

					if (FloatRect(imgui::GetItemRectMin(), imgui::GetItemRectSize()).contains(Vector2f(logicIO.mousePos))) {
						if (logicIO.mouseState[Mouse::Right] == LogicIO::JustPressed) {
							imgui::OpenPopup("ItemContextMenu");
						}
						if (!contextMenuOpen&&info.id != "none") {
							imgui::BeginTooltip();
							imgui::TextUnformatted(text.get(slotName + ".name"));
							const string& desc = text.getstr(slotName + ".desc");
							if (desc != "") {
								imgui::PushStyleColor(ImGuiCol_Text, Color(220, 220, 220));
								imgui::TextUnformatted(desc.c_str());
								imgui::PopStyleColor();
							}
							imgui::PushStyleColor(ImGuiCol_Text, imgui::GetStyleColorVec4(ImGuiCol_TextDisabled));
							imgui::Text(text.get("inventory.maxcount"), maxItemsThisSlot);
							imgui::PopStyleColor();
							imgui::EndTooltip();
						}
					}

					if (imgui::BeginPopup("ItemContextMenu")) {
						thisFrameHasContextMenu = true;
						imgui::PopStyleColor(7);
						imgui::PopStyleVar(2);
						static char charbuf[256] = {};
						strcpy(charbuf, dataset[to_string(i) + to_string(j) + "item_name"].getDataString().c_str());
						imgui::InputText("Item Id", charbuf, 256);
						dataset[to_string(i) + to_string(j) + "item_name"].getDataString() = charbuf;
						imgui::InputInt("Count", &dataset[to_string(i) + to_string(j) + "count"].getDataInt());
						if (imgui::TreeNodeEx("Other Datasets", ImGuiTreeNodeFlags_DefaultOpen)) {
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
									pushBoolText(d.getDataBool());
								}
								if (imgui::Button("Add! (With Prefix Added)", ImVec2(-1, 0))) {
									dataset.getDatasets().insert(make_pair(slotPrefix + namebuf, d));
									namebuf[0] = '\0';
									d = Data();
									type = 0;
								}
								imgui::EndMenu();
							}
							for (auto j = dataset.getDatasets().begin(); j != dataset.getDatasets().end();) {
								if (j->first.substr(0, slotPrefix.size()) != slotPrefix || j->first == slotPrefix + "item_name" || j->first == slotPrefix + "count") {
									j++;
									continue;
								}
								auto& i = *j;
								static float buttonHeight = .0f;
								if (i.second.getType() == Data::Integer)
									imgui::InputInt(i.first.substr(slotPrefix.size()).c_str(), &i.second.getDataInt());
								else if (i.second.getType() == Data::String) {
									static char buf[128];
									strcpy(buf, i.second.getDataString().c_str());
									imgui::InputText(i.first.substr(slotPrefix.size()).c_str(), buf, 128);
									i.second.getDataString() = buf;
								}
								else if (i.second.getType() == Data::Bool) {
									imgui::Selectable(i.first.substr(slotPrefix.size()).c_str(), &i.second.getDataBool(), 0, ImVec2(0, buttonHeight));
									pushBoolText(i.second.getDataBool());
								}
								else if (i.second.getType() == Data::Uuid)
									imgui::Text("{%s} :%s", i.second.getDataUuid().toString(), i.first.substr(slotPrefix.size()).c_str());
								imgui::SameLine(16.0f);
								if (imgui::Button(("X##Remove" + i.first.substr(slotPrefix.size())).c_str()))
									j = dataset.getDatasets().erase(j);
								else
									j++;
								if (imgui::IsItemHovered()) {
									imgui::BeginTooltip();
									imgui::Text("Remove This Data Entry");
									imgui::EndTooltip();
								}
								buttonHeight = imgui::GetItemRectSize().y;
							}

							imgui::TreePop();
						}
						imgui::EndPopup();

						imgui::PushStyleColor(ImGuiCol_ModalWindowDarkening, Color(0, 0, 0, 128));
						imgui::PushStyleColor(ImGuiCol_Button, Color::Transparent);
						imgui::PushStyleColor(ImGuiCol_ButtonHovered, Color(255, 255, 255, 64));
						imgui::PushStyleColor(ImGuiCol_ButtonActive, Color(255, 255, 255, 48));
						imgui::PushStyleColor(ImGuiCol_FrameBg, Color(0, 0, 0, 128));
						imgui::PushStyleColor(ImGuiCol_PopupBg, Color(0, 0, 0, 128));
						imgui::PushStyleColor(ImGuiCol_TitleBgActive, imgui::GetStyleColorVec4(ImGuiCol_PopupBg));
						imgui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
						imgui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					}

					imgui::PopID();
				}
			}
			imgui::PopStyleColor(7);
			imgui::PopStyleVar(2);
			imgui::PopItemFlag();

			contextMenuOpen = thisFrameHasContextMenu;
		}
	}
	imgui::End();
}


////////////////////////////////////////
void MapEditorScene::stop() {

}

