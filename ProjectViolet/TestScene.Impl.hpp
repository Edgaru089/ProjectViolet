
#include "TestScene.hpp"

#include "TerrainManager.hpp"
#include "Uuid.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"
#include "TestEntity.hpp"
#include "ParticleSystem.hpp"
#include "EntityAllocator.hpp"
#include "InGameUI.hpp"
#include "PlayerInventory.hpp"
#include "GamePausedUI.hpp"


////////////////////////////////////////
void TestScene::preWindowInitalaize() {
	mlog << "[TestScene] PreWindowInitalaize Calling..." << dlog;

	mlog << "[TestScene] Initalaizing Systems..." << dlog;
	assetManager.loadListFile();
	textureManager.bindTexture();

	blockAllocator.initalaize();
	entityAllocator.initalaize();
	itemAllocator.initalaize();

	mlog << "[TestScene] Initalazing NovelGame..." << dlog;
	novelGameSystem.preInitalaize();

	mlog << "[TestScene] Loading Fonts..." << dlog;
	//imgui::GetIO().Fonts->AddFontFromFileTTF(assetManager.getAssetFilename("font_minecraft").c_str(),
	//	16, nullptr, imgui::GetIO().Fonts->GetGlyphRangesDefault());
	//imgui::GetIO().Fonts->AddFontFromFileTTF(assetManager.getAssetFilename("courier_new").c_str(),
	//	13, nullptr, imgui::GetIO().Fonts->GetGlyphRangesDefault());
	//imgui::GetIO().Fonts->AddFontFromFileTTF(assetManager.getAssetFilename("courier_new_bold").c_str(),
	//	13, nullptr, imgui::GetIO().Fonts->GetGlyphRangesDefault());
	//imgui::GetIO().Fonts->AddFontFromFileTTF(assetManager.getAssetFilename("source_han_sans").c_str(),
	//	16, nullptr, imgui::GetIO().Fonts->GetGlyphRangesChinese());
	//imgui::GetIO().Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Dengb.ttf",
	//										 16, nullptr, imgui::GetIO().Fonts->GetGlyphRangesChinese());
	mlog << "[TestScene] Updating Font Textures..." << dlog;
	imgui::SFML::UpdateFontTexture();

	background.loadFromFile(assetManager.getAssetFilename("background_stone"));
	background.setRepeated(true);

	win.setKeyRepeatEnabled(false);

	mlog << "[TestScene] PreWindowInitalaize Done." << dlog;
}


////////////////////////////////////////
void TestScene::postWindowInitalaize(RenderWindow& win) {
	mlog << "[TestScene] PostWindowInitalaize Calling..." << dlog;

	// There's nothing to do!

	mlog << "[TestScene] PostWindowInitalaize Done." << dlog;
}


////////////////////////////////////////
void TestScene::start(RenderWindow & win) {
	mlog << "[TestScene] Scene Starting..." << dlog;
	testEntity = Uuid::nil();

	prov.setup(Vector2u(3, 2), 5);

	localPlayer = make_shared<PlayerEntity>();
	localPlayer->setIsLocalPlayer(true);
	localPlayer->setHealth(localPlayer->getMaxHealth());
	entityManager.insert(localPlayer, Vector2d(prov.getSpawnPoints()[0]) + Vector2d(0.5, 1 - 1e-7));

	renderIO.gameScaleFactor = 48.0;
	gameIO.ruleExplosionDamagesTerrain = true;
	showDebugInfo = false;
	showExtraImGuiWindows = false;

	uiManager.changeUI(nullptr);

	mlog << "[TestScene] Scene Started!" << dlog;
}


////////////////////////////////////////
void TestScene::onRender(RenderWindow & win) {
	AUTOLOCK_ALL_SYSTEM;

	// Place screen offset calculations here to obtain maxinum synchronization
	if (localPlayer != nullptr) {
		Vector2d onScrPlayer = localPlayer->getPosition()*renderIO.gameScaleFactor;
		renderIO.gameScenePosOffset = Vector2d(logicIO.renderSize) / 2.0 - onScrPlayer;
	}

	Clock drawTime;

	Sprite sp;
	sp.setTexture(background);
	sp.setScale(renderIO.gameScaleFactor / background.getSize().x * 1.5, renderIO.gameScaleFactor / background.getSize().y * 1.5);
	sp.setTextureRect(IntRect(0, 0, (win.getView().getSize().x + 2 * renderIO.gameScaleFactor * 1.5) / sp.getScale().x,
		(win.getView().getSize().y + 2 * renderIO.gameScaleFactor * 1.5) / sp.getScale().y));
	sp.setPosition(fmod(renderIO.gameScenePosOffset.x, renderIO.gameScaleFactor * 1.5) - renderIO.gameScaleFactor * 1.5,
		fmod(renderIO.gameScenePosOffset.y, renderIO.gameScaleFactor * 1.5) - renderIO.gameScaleFactor * 1.5);
	win.draw(sp);

	renderIO.viewRect = FloatRect(-renderIO.gameScenePosOffset.x, -renderIO.gameScenePosOffset.y, logicIO.renderSize.x, logicIO.renderSize.y);
	renderIO.viewRectBlock = DoubleRect(
		renderIO.viewRect.left / renderIO.gameScaleFactor,
		renderIO.viewRect.top / renderIO.gameScaleFactor,
		renderIO.viewRect.width / renderIO.gameScaleFactor,
		renderIO.viewRect.height / renderIO.gameScaleFactor);
	win.setView(View(renderIO.viewRect));

	VertexArray totalTriangles;

	totalTriangles.setPrimitiveType(PrimitiveType::Triangles);
	terrainManager.getRenderList(totalTriangles); terrainListSize = totalTriangles.getVertexCount();
	terrainManager.getLightMask(totalTriangles); terrainMaskSize = totalTriangles.getVertexCount()
		- terrainListSize;
	entityManager.getRenderList(totalTriangles); entityListSize = totalTriangles.getVertexCount()
		- terrainListSize - terrainMaskSize;
	particleSystem.getRenderList(totalTriangles); particleListSize = totalTriangles.getVertexCount()
		- terrainListSize - terrainMaskSize - entityListSize;
	totalVertSize = totalTriangles.getVertexCount();

	win.draw(totalTriangles, textureManager.getBindingTexture());

	for (auto i : entityManager.getEntityMapList())
		if (i.second != nullptr)
			win.draw(renderRect(i.second->getHitbox()*renderIO.gameScaleFactor));

	Vector2i mousePos = TerrainManager::convertScreenPixelToWorldBlockCoord(logicIO.mousePos);
	shared_ptr<Block> b = terrainManager.getBlock(mousePos);
	if (b == nullptr) {
		Vector2d center = Vector2d(mousePos.x + 0.5, mousePos.y + 0.5)*renderIO.gameScaleFactor;
		double width = renderIO.gameScaleFactor;
		VertexArray verts;
		// Left-Top
		verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y - width / 2.0)));
		// Right-Top
		verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - width / 2.0)));
		// Right-Bottom
		verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y + width / 2.0)));
		// Left-Bottom
		verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y + width / 2.0)));
		// Left-Top
		verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y - width / 2.0)));

		verts.setPrimitiveType(PrimitiveType::LinesStrip);
		win.draw(verts);
	}
	else
		win.draw(renderRect(b->getHitbox()*renderIO.gameScaleFactor));

	if (localPlayer != nullptr) {
		VertexArray verts;
		Vector2d eye = localPlayer->getEyePosition();
		//verts.append(Vertex(Vector2f(eye.x, eye.y - logicIO.renderSize.y / (float)renderIO.gameScaleFactor / 2.0f) * (float)renderIO.gameScaleFactor, Color::White));
		//verts.append(Vertex(Vector2f(eye.x, eye.y + logicIO.renderSize.y / (float)renderIO.gameScaleFactor / 2.0f) * (float)renderIO.gameScaleFactor, Color::White));
		//verts.append(Vertex(Vector2f(eye.x - logicIO.renderSize.x / (float)renderIO.gameScaleFactor / 2.0f, eye.y) * (float)renderIO.gameScaleFactor, Color::White));
		//verts.append(Vertex(Vector2f(eye.x + logicIO.renderSize.x / (float)renderIO.gameScaleFactor / 2.0f, eye.y) * (float)renderIO.gameScaleFactor, Color::White));
		verts.append(Vertex(Vector2f(eye) * (float)renderIO.gameScaleFactor, Color::White));
		verts.append(Vertex(Vector2f(TerrainManager::convertScreenPixelToWorldCoord(logicIO.mousePos)) * (float)renderIO.gameScaleFactor, Color::White));
		verts.setPrimitiveType(PrimitiveType::Lines);
		win.draw(verts);
	}

	win.setView(View(FloatRect(0, 0, logicIO.renderSize.x, logicIO.renderSize.y)));
	VertexArray verts;
	Vector2i pos = logicIO.mousePos;
	//verts.append(Vertex(Vector2f(pos.x, 0)));
	//verts.append(Vertex(Vector2f(pos.x, win.getView().getSize().y)));
	//verts.append(Vertex(Vector2f(0, pos.y)));
	//verts.append(Vertex(Vector2f(win.getView().getSize().x, pos.y)));
	verts.append(Vertex(Vector2f(pos.x, pos.y - 15)));
	verts.append(Vertex(Vector2f(pos.x, pos.y + 15)));
	verts.append(Vertex(Vector2f(pos.x - 15, pos.y)));
	verts.append(Vertex(Vector2f(pos.x + 15, pos.y)));
	verts.setPrimitiveType(PrimitiveType::Lines);
	win.draw(verts);

}


////////////////////////////////////////
bool TestScene::_sendMousePressedToHandItem(Mouse::Button button) {
	bool flag = false;
	const string& name = playerInventory.slots[0][playerInventory.cursorId]["item_name"];
	if (name.substr(0, 5) == "item_") {
		shared_ptr<Item> item = itemAllocator.allocate(name.substr(5), playerInventory.slots[0][playerInventory.cursorId]);
		if (item != nullptr && (button == Mouse::Left ? item->_onLeftPressed() : item->_onRightPressed()))
			flag = true;
	}
	return flag;
}


////////////////////////////////////////
void TestScene::_sendMouseReleasedToHandItem(Mouse::Button button) {
	const string& name = playerInventory.slots[0][playerInventory.cursorId]["item_name"];
	if (name.substr(0, 5) == "item_") {
		shared_ptr<Item> item = itemAllocator.allocate(name.substr(5), playerInventory.slots[0][playerInventory.cursorId]);
		if (item != nullptr) {
			if (button == Mouse::Left)
				item->_onLeftReleased();
			else
				item->_onRightReleased();
		}
	}
}


////////////////////////////////////////
void TestScene::updateLogic(RenderWindow & win) {
	AUTOLOCK_ALL_SYSTEM;

	static auto handleKeyState = [&](LogicIO::KeyState& state, bool isDown) {
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

	if (uiManager.getCurrentUI() == nullptr || !uiManager.getCurrentUI()->isBlocking()) {
		// Update key states
		// Keyboard
		for (int i = 0; i < Keyboard::KeyCount; i++)
			handleKeyState(logicIO.keyboardState[i], Keyboard::isKeyPressed((Keyboard::Key)i));
		// Mouse
		for (int i = 0; i < Mouse::ButtonCount; i++)
			handleKeyState(logicIO.mouseState[i], Mouse::isButtonPressed((Mouse::Button)i));

		logicIO.mousePos = Mouse::getPosition(win);

		if (localPlayer != nullptr) {
			Vector2d mouseBlock = TerrainManager::convertScreenPixelToWorldCoord(logicIO.mousePos);
			Vector2d eyePos = localPlayer->getEyePosition();
			gameIO.degreeAngle = atan((mouseBlock.y - eyePos.y) / (mouseBlock.x - eyePos.x)) * 180.0 / PI;
			if (mouseBlock.x - eyePos.x < 0)
				gameIO.degreeAngle += 180.0;
			if (gameIO.degreeAngle > 360.0)
				gameIO.degreeAngle -= 360.0;
			if (gameIO.degreeAngle < 0.0)
				gameIO.degreeAngle += 360.0;
		}

		// Manage keys and game/player controls
		// Mouse controls
		if (!imgui::GetIO().WantCaptureMouse && logicIO.hasFocus && (logicIO.mouseState[Mouse::Left] == LogicIO::JustPressed))
			if (!_sendMousePressedToHandItem(Mouse::Left))
				terrainManager.breakBlock(TerrainManager::convertScreenPixelToWorldBlockCoord(logicIO.mousePos));
		if (logicIO.mouseState[Mouse::Left] == LogicIO::JustReleased)
			_sendMouseReleasedToHandItem(Mouse::Left);
		if (!imgui::GetIO().WantCaptureMouse && logicIO.hasFocus && (logicIO.mouseState[Mouse::Right] == LogicIO::JustPressed)) {
			const string& str = playerInventory.slots[0][playerInventory.cursorId]["item_name"].getDataString();
			bool sendRightClick = true;
			Vector2i coord = TerrainManager::convertScreenPixelToWorldBlockCoord(logicIO.mousePos);
			shared_ptr<Block> b = terrainManager.getBlock(coord);
			if (str != "") {
				if (str.substr(0, 6) == "block_")
					terrainManager.placeBlock(coord, str.substr(6));
				else if (str.substr(0, 5) == "item_") {
					shared_ptr<Item> item = itemAllocator.allocate(str.substr(5), playerInventory.slots[0][playerInventory.cursorId]);
					if (item != nullptr)
						sendRightClick = !item->_onRightPressed();
				}
			}
			if (sendRightClick) {
				if (b != nullptr)
					b->_onRightClick();
			}
		}
		if (logicIO.mouseState[Mouse::Right] == LogicIO::JustReleased)
			_sendMouseReleasedToHandItem(Mouse::Right);
		if (!imgui::GetIO().WantCaptureMouse && logicIO.hasFocus && (logicIO.mouseState[Mouse::Middle] == LogicIO::JustPressed)) {
			if (!localPlayer->isAlive()) {
				localPlayer = make_shared<PlayerEntity>();
				localPlayer->setHealth(localPlayer->getMaxHealth());
				localPlayer->setIsLocalPlayer(true);
				entityManager.insert(localPlayer, TerrainManager::convertScreenPixelToWorldCoord(logicIO.mousePos));
			}
			else {
				if (testEntity == Uuid()) {
					shared_ptr<TestEntity> te = make_shared<TestEntity>();
					te->setHealth(te->getMaxHealth());
					testEntity = entityManager.insert(te, TerrainManager::convertScreenPixelToWorldCoord(logicIO.mousePos));
				}
				else {
					shared_ptr<TestEntity> te = make_shared<TestEntity>();
					te->setHealth(te->getMaxHealth());
					entityManager.insert(te, TerrainManager::convertScreenPixelToWorldCoord(logicIO.mousePos));
				}
				//entityManager.explode(TerrainManager::convertScreenPixelToWorldCoord(logicIO.mousePos),
				//					  12.0, false);
			//particleSystem.emitSmoke(TerrainManager::convertScreenPixelToWorldCoord(logicIO.mousePos));
			//for (int i = 1; i <= 1000; i++) {
			//	// Tile Drop
			//	ItemEntity* e = new ItemEntity("block_stone");
			//	// Give a random velocity
			//	e->accelerateVector(1.0, 180 + rand() % 180);

			//	entityManager.insert(e, Vector2d(TerrainManager::convertScreenPixelToWorldCoord(logicIO.mousePos)) + Vector2d(0.5, 0.8));
			//}
			}
		}

		// Keyboard controls
		if (!imgui::GetIO().WantCaptureKeyboard && logicIO.hasFocus && logicIO.hasFocus &&
			logicIO.keyboardState[Keyboard::F1] == LogicIO::JustPressed)
			showExtraImGuiWindows = !showExtraImGuiWindows;
		if (!imgui::GetIO().WantCaptureKeyboard && logicIO.hasFocus &&
			logicIO.keyboardState[Keyboard::F3] == LogicIO::JustPressed)
			app->hasLog = showDebugInfo = !showDebugInfo;
		if (!imgui::GetIO().WantCaptureKeyboard && logicIO.hasFocus &&
			(logicIO.keyboardState[Keyboard::A] == LogicIO::JustPressed || logicIO.keyboardState[Keyboard::Left] == LogicIO::JustPressed))
			localPlayer->moveLeft(true);
		if (logicIO.keyboardState[Keyboard::A] == LogicIO::JustReleased || logicIO.keyboardState[Keyboard::Left] == LogicIO::JustReleased)
			localPlayer->moveLeft(false);
		if (!imgui::GetIO().WantCaptureKeyboard && logicIO.hasFocus &&
			(logicIO.keyboardState[Keyboard::D] == LogicIO::JustPressed || logicIO.keyboardState[Keyboard::Right] == LogicIO::JustPressed))
			localPlayer->moveRight(true);
		if (logicIO.keyboardState[Keyboard::D] == LogicIO::JustReleased || logicIO.keyboardState[Keyboard::Right] == LogicIO::JustReleased)
			localPlayer->moveRight(false);
		if (!imgui::GetIO().WantCaptureKeyboard && logicIO.hasFocus &&
			(Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)))
			localPlayer->ascendLadder(true);
		if (logicIO.keyboardState[Keyboard::W] == LogicIO::JustReleased || logicIO.keyboardState[Keyboard::Up] == LogicIO::JustReleased)
			localPlayer->ascendLadder(false);
		if (!imgui::GetIO().WantCaptureKeyboard && logicIO.hasFocus &&
			(Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down)))
			localPlayer->decendLadder(true);
		if (logicIO.keyboardState[Keyboard::S] == LogicIO::JustReleased || logicIO.keyboardState[Keyboard::Down] == LogicIO::JustReleased)
			localPlayer->decendLadder(false);
		if (!imgui::GetIO().WantCaptureKeyboard && logicIO.hasFocus &&
			(logicIO.keyboardState[Keyboard::LControl] == LogicIO::JustPressed))
			localPlayer->crouch(true);
		if (logicIO.keyboardState[Keyboard::LControl] == LogicIO::JustReleased)
			localPlayer->crouch(false);
		if (!imgui::GetIO().WantCaptureKeyboard && logicIO.hasFocus &&
			Keyboard::isKeyPressed(Keyboard::Space))
			localPlayer->jump();

		terrainManager.updateLogic();
		particleSystem.updateLogic();
		entityManager.updateLogic();
		playerInventory.updateLogic();

		novelGameSystem.updateLogic();
	}
	else // Excilpty handle the Escape key to support escaping from pause screen using Esc key
		handleKeyState(logicIO.keyboardState[Keyboard::Escape], Keyboard::isKeyPressed(Keyboard::Escape));

	uiManager.updateLogic();
	if (logicIO.keyboardState[Keyboard::E] == LogicIO::JustPressed && !imgui::GetIO().WantCaptureKeyboard && logicIO.hasFocus)
		uiManager.changeUI(make_shared<PlayerInventoryUI>());
	if (logicIO.keyboardState[Keyboard::Escape] == LogicIO::JustPressed && !imgui::GetIO().WantCaptureKeyboard && logicIO.hasFocus)
		uiManager.changeUI(make_shared<GamePausedUI>());
}


////////////////////////////////////////
void TestScene::handleEvent(RenderWindow & win, Event & event) {
	AUTOLOCK_ALL_SYSTEM;

	if (event.type == Event::MouseWheelScrolled) {
		if (Mouse::isButtonPressed(Mouse::Left))
			_sendMouseReleasedToHandItem(Mouse::Left);
		if (Mouse::isButtonPressed(Mouse::Right))
			_sendMouseReleasedToHandItem(Mouse::Right);
		if (event.mouseWheelScroll.delta < 0) {
			playerInventory.addCursorId();
		}
		else /*if (event.mouseWheelScroll.delta > 0)*/ {
			playerInventory.minusCursorId();
		}
		if (Mouse::isButtonPressed(Mouse::Left))
			_sendMousePressedToHandItem(Mouse::Left);
		if (Mouse::isButtonPressed(Mouse::Right))
			_sendMousePressedToHandItem(Mouse::Right);
	}
}


////////////////////////////////////////
void TestScene::runImGui() {
	AUTOLOCK_ALL_SYSTEM;

	if (showExtraImGuiWindows) {
		imgui::Begin("TestEntity");
		if (testEntity == Uuid::nil())
			imgui::Text("TextEntity == nullptr");
		else {
			shared_ptr<Entity> e = entityManager.getEntity(testEntity);
			if (e == nullptr)
				testEntity = Uuid();
			else {
				TestEntity& te = dynamic_cast<TestEntity&>(*e);
				imgui::Text("Health: %d / %d", te.getHealth(), te.getMaxHealth());
			}
		}
		imgui::End();

		imgui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		imgui::ShowFontSelector("Fonts");
		imgui::Text(u8"Innovation In China 中国智造，惠及全球 1234567890");
		static char langFile[128] = { "lang-zh-Hans.list" };
		if (imgui::InputText("Language File", langFile, sizeof(langFile), ImGuiInputTextFlags_EnterReturnsTrue))
			text.loadFromFile(langFile);
		static float value = renderIO.gameScaleFactor;
		imgui::SliderFloat("GameScaleFactor", &value, 16, 64);
		renderIO.gameScaleFactor = value;
		imgui::Checkbox("Explosion damages terrain", &gameIO.ruleExplosionDamagesTerrain);
		if (imgui::Button("Break!"))
			for (auto& k : terrainManager.getChunks()) {
				Vector2i off = k.first*chunkSize;
				shared_ptr<Chunk> c = k.second;
				for (int i = 0; i < chunkSize; i++)
					for (int j = 0; j < chunkSize; j++) {
						if (c->getBlock(Vector2i(i, j)) == nullptr)
							continue;
						if (c->getBlock(Vector2i(i, j))->getBlockId() != "bedrock")
							terrainManager.breakBlock(off + Vector2i(i, j));
					}
			}
		imgui::SameLine();
		if (imgui::Button("Start NovelGame"))
			novelGameSystem.start("default");
		imgui::Image(*textureManager.getBindingTexture());
		imgui::End();

		imgui::ShowDemoWindow();
	}

	if (showDebugInfo) {
		ENTER_IMGUI_DEBUG;

		auto pushBoolText = [](bool value, bool sameLine = true) {
			if (sameLine)
				imgui::SameLine();
			if (value)
				imgui::PushStyleColor(ImGuiCol_Text, Color::Green);
			else
				imgui::PushStyleColor(ImGuiCol_Text, Color::Red);
			imgui::Text("%s", value ? "True" : "False");
			imgui::PopStyleColor();
		};

		imgui::Text("%s %s / Version %d.%d.%d %s / Compile Time %s",
			projectCode.c_str(), projectSuffix.c_str(), majorVersion, minorVersion, patchVersion, releaseStage.c_str(), compileTime.c_str());
#ifdef USE_ASYNC_RENDERING
		imgui::Text("State: Async-Rendering, TPS: %d, EPS: %d, FPS: %d", logicTickPerSecond, eventTickPerSecond, framePerSecond);
#else
		imgui::Text("State: Mono-Thread, FPS: %d", eventTickPerSecond);
#endif // USE_ASYNC_RENDERING
		imgui::Text("Entity Count: %d", entityManager.getEntityMapList().size());
		imgui::Text("Particle Count: %d", particleSystem.getParticleList().size());

		imgui::Text("PlayerEntity Info");
		if (localPlayer != nullptr) {
			imgui::Text("  Uuid: {%s}", localPlayer->getUuid().toString().c_str());
			imgui::Text("  Position: (%.3lf, %.3lf)", localPlayer->getPosition().x, localPlayer->getPosition().y);
			imgui::Text("  Velocity: (%.3lf, %.3lf)", localPlayer->getVelocity().x, localPlayer->getVelocity().y);
			imgui::Text("  EyePosition: (%.3lf, %.3lf)", localPlayer->getEyePosition().x, localPlayer->getEyePosition().y);
			imgui::Text("  IsOnGround:"); pushBoolText(localPlayer->isOnGround());
			imgui::Text("  IsOnLadder:"); pushBoolText(localPlayer->isOnLadder());
			imgui::Text("  IsCrouched:"); pushBoolText(localPlayer->isCrouched());
			imgui::Text("  IsStuck(WantStandup):"); pushBoolText(localPlayer->isStuck());
		}
		else
			imgui::Text("  localPlayer == nullptr");

		imgui::Text("Mouse Cursor Info");
		Vector2i pos = logicIO.mousePos;
		Vector2i posB = TerrainManager::convertScreenPixelToWorldBlockCoord(pos);
		Vector2d posC = TerrainManager::convertScreenPixelToWorldCoord(pos);
		Vector2i chunkId = TerrainManager::convertWorldCoordToChunkId(posB);
		Vector2i inChunkC = TerrainManager::convertWorldCoordToInChunkCoord(posB);
		shared_ptr<Chunk> c = terrainManager.getChunk(chunkId);
		shared_ptr<Block> b = terrainManager.getBlock(posB);
		imgui::Text("  OnScreen Pos: (%d, %d)", pos.x, pos.y);
		imgui::Text("  World Pos: (%.3lf, %.3lf)", posC.x, posC.y);
		imgui::Text("  DegreeAngle: %.3lf", gameIO.degreeAngle);
		imgui::Text("  World Block Pos: (%d, %d)", posB.x, posB.y);
		imgui::Text("  InChunk Pos: Chunk(%d, %d), InChunk(%d, %d)", chunkId.x, chunkId.y, inChunkC.x, inChunkC.y);
		imgui::Text("  BlockType: %s", b == nullptr ? "nullptr" : b->getBlockId().c_str());
		if (c != nullptr)
			imgui::Text("  Light Level: %d", c->lightLevel[inChunkC.x][inChunkC.y]);
		if (b != nullptr) {
			for (auto& i : b->getDataset().getDatasets()) {
				imgui::Text("  %s:", i.first.c_str()); imgui::SameLine();
				if (i.second.getType() == Data::Integer)
					imgui::Text("%d", i.second.getDataInt());
				else if (i.second.getType() == Data::String)
					imgui::Text("%s", i.second.getDataString().c_str());
				else if (i.second.getType() == Data::Bool)
					pushBoolText(i.second.getDataBool(), false);
				else if (i.second.getType() == Data::Uuid)
					imgui::Text("{%s}", i.second.getDataUuid().toString());
			}
		}

		imgui::Text("Item In Hand");
		imgui::Text("  CursorId: %d", playerInventory.cursorId);
		Dataset& slot = playerInventory.slots[0][playerInventory.cursorId];
		for (auto& i : slot.getDatasets()) {
			imgui::Text("  %s:", i.first.c_str()); imgui::SameLine();
			if (i.second.getType() == Data::Integer)
				imgui::Text("%d", i.second.getDataInt());
			else if (i.second.getType() == Data::String)
				imgui::Text("%s", i.second.getDataString().c_str());
			else if (i.second.getType() == Data::Bool)
				pushBoolText(i.second.getDataBool(), false);
			else if (i.second.getType() == Data::Uuid)
				imgui::Text("{%s}", i.second.getDataUuid().toString());
		}

		imgui::Text("TerrainRender:  %d verts (%d tri)",
			terrainListSize, terrainListSize / 3);
		imgui::Text("TerrainLight:   %d verts (%d tri)",
			terrainMaskSize, terrainMaskSize / 3);
		imgui::Text("ParticleRender: %d verts (%d tri)",
			particleListSize, particleListSize / 3);
		imgui::Text("EntityRender:   %d verts (%d tri)",
			entityListSize, entityListSize / 3);
		imgui::Text("Total:          %d verts (%d tri)",
			totalVertSize, totalVertSize / 3);

		imgui::Text("Tick Time");
		imgui::Text("  Render");
		Time values[] = { appRenderTime, runImGuiTime, imGuiRenderTime, imGuiUpdateTime };
		string names[] = { "App->OnRender() %6dMs", "App->RunImGui() %6dMs", "ImGui::Render() %6dMs", "ImGui::Update() %6dMs" };
		for (int i = 0; i < 4; i++) {
			imgui::Text(names[i].c_str(), values[i].asMicroseconds());
			if (values[i] != Time::Zero) {
				imgui::SameLine();
				imgui::ProgressBar(1.0f, ImVec2(values[i].asMicroseconds() / 10.0f, 13.0f), "");
			}
		}
		imgui::Text("App->UpdateLogic() %3dMs", logicThreadTickTime.asMicroseconds());
		if (logicThreadTickTime != Time::Zero) {
			imgui::SameLine();
			imgui::ProgressBar(1.0f, ImVec2(logicThreadTickTime.asMicroseconds() / 10.0f, 13.0f), "");
		}
		LEAVE_IMGUI_DEBUG;
	}

	uiManager.runImGui();

	playerInventory.runImGui();

	novelGameSystem.runImGui();
}


////////////////////////////////////////
void TestScene::stop() {

}


////////////////////////////////////////
