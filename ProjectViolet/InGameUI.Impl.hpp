#pragma once

#include "InGameUI.hpp"
#include "PlayerInventory.hpp"
#include "ItemAllocator.hpp"
#include "TextSystem.hpp"


////////////////////////////////////////
InGameUIManager::~InGameUIManager() {
}


////////////////////////////////////////
void InGameUIManager::updateLogic() {
	AUTOLOCKABLE(*this);
	if (wantChange) {
		if (curUI != nullptr)
			curUI->_onClose();
		curUI = wantChangeUI;
		if (curUI != nullptr)
			curUI->_onOpen();
		wantChange = false;
	}

	if (curUI == nullptr)
		return;

	if (logicIO.keyboardState[Keyboard::Escape] == LogicIO::JustPressed ||
		logicIO.keyboardState[Keyboard::E] == LogicIO::JustPressed) {
		curUI = nullptr;
	}
}


////////////////////////////////////////
void InGameUIManager::runImGui() {
	AUTOLOCKABLE(*this);

	if (curUI == nullptr)
		return;

	imgui::PushStyleColor(ImGuiCol_ModalWindowDarkening, Color(0, 0, 0, 128));
	imgui::PushStyleColor(ImGuiCol_Button, Color::Transparent);
	imgui::PushStyleColor(ImGuiCol_ButtonHovered, Color(255, 255, 255, 64));
	imgui::PushStyleColor(ImGuiCol_ButtonActive, Color(255, 255, 255, 48));
	imgui::PushStyleColor(ImGuiCol_FrameBg, Color(0, 0, 0, 128));
	imgui::PushStyleColor(ImGuiCol_PopupBg, Color(0, 0, 0, 128));
	imgui::PushStyleColor(ImGuiCol_TitleBgActive, imgui::GetStyleColorVec4(ImGuiCol_PopupBg));
	imgui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	imgui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	GImGui->ModalWindowDarkeningRatio = 1.0f;
	ImGui::OpenPopup(curUI->windowTitle().c_str());

	if (!curUI->showInventory())
		curUI->runImGui();

	if (curUI->showInventory()) {
		FloatRect winRect(-1, -1, -1, -1);
		imgui::SetNextWindowPos(
			ImVec2(imgui::GetIO().DisplaySize.x / 2.0f, imgui::GetIO().DisplaySize.y / 2.0f),
			ImGuiCond_Always,
			ImVec2(0.5f, 0.5f));
		if (imgui::BeginPopupModal(curUI->windowTitle().c_str(), nullptr,
								   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove)) {
			curUI->runImGui();

			_runInventoryUI();

			// Cursor item
			TextureInfo info = textureManager.getTextureInfo(localPlayer->getDataset()["cursor_item_name"].getDataString());
			if (info.vaild) {
				ImVec2 pos = imgui::GetIO().MousePos;
				imgui::GetOverlayDrawList()->AddImage((ImTextureID)info.texture->getNativeHandle(),
					ImVec2(pos.x - 16, pos.y - 16), ImVec2(pos.x + 16, pos.y + 16),
					ImVec2(info.textureRect.left / (float)info.texture->getSize().x,
						   info.textureRect.top / (float)info.texture->getSize().y),
					ImVec2((info.textureRect.left + info.textureRect.width) / (float)info.texture->getSize().x, (
						info.textureRect.top + info.textureRect.height) / (float)info.texture->getSize().y));
				pos.x -= 16 + 3 - 2; pos.y -= 16 + 3;
				if (localPlayer->getDataset()["cursor_count"].getDataInt() != 1)
					imgui::GetOverlayDrawList()->AddText(pos, ImU32(0xFFFFFFFF), StringParser::toString(localPlayer->getDataset()["cursor_count"].getDataInt()).c_str());
			}

			winRect = FloatRect(imgui::GetWindowPos(), imgui::GetWindowSize());

			imgui::EndPopup();
		}
		if (winRect != FloatRect(-1, -1, -1, -1) && !winRect.contains(Vector2f(logicIO.mousePos)) && logicIO.mouseState[Mouse::Left] == LogicIO::JustReleased) {
			// Throw cursor item
			Dataset& dataset = localPlayer->getDataset();
			string prefix = "cursor_";
			string& name = dataset[prefix + "item_name"].getDataString();
			int& count = dataset[prefix + "count"].getDataInt();
			if (!name.empty() && count > 0) {
				shared_ptr<ItemEntity> e = make_shared<ItemEntity>(name);
				for (auto& i : dataset.getDatasets())
					if (i.first.substr(0, prefix.size()) == prefix)
						e->getDataset().getDatasets().insert(make_pair(i.first.substr(prefix.size()), i.second));
				assert(e->getData("item_name").getDataString() == name);
				assert(e->getData("count").getDataInt() == count);
				e->accelerateVector(2.0, gameIO.degreeAngle); // Give a velocity
				e->throwCooldownMilli() = 1000;
				entityManager.insert(e, localPlayer->getEyePosition() + Vector2d(.0, e->getSize().y / 2.0));
				name.clear();
				count = 0;
			}
		}
	}

	imgui::PopStyleColor(7);
	imgui::PopStyleVar(2);
}


////////////////////////////////////////
void InGameUIManager::_runInventoryUI() {
	imgui::PopStyleVar();
	imgui::TextUnformatted("Inventory");
	imgui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	// Player inventory slots
	for (int i = 1; i <= 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (j != 0)
				imgui::SameLine();
			PlayerInventoryUI::ImGuiInventorySlot(localPlayer->getDataset(), i, j);
		}
	}
	imgui::Dummy(ImVec2(.0f, 6.0f));
	for (int j = 0; j < 9; j++) {
		if (j != 0)
			imgui::SameLine();
		PlayerInventoryUI::ImGuiInventorySlot(localPlayer->getDataset(), 0, j);
	}
}


////////////////////////////////////////
void PlayerInventoryUI::ImGuiInventorySlot(Dataset& dataset, int i, int j, int extraPushId) {
	string slotPrefix = to_string(i) + to_string(j);
	string slotName = dataset[to_string(i) + to_string(j) + "item_name"].getDataString(),
		cursorName = localPlayer->getDataset()["cursor_item_name"].getDataString();
	int slotCount = dataset[to_string(i) + to_string(j) + "count"].getDataInt(),
		cursorCount = localPlayer->getDataset()["cursor_count"].getDataInt();

	TextureInfo info = textureManager.getTextureInfo(dataset[to_string(i) + to_string(j) + "item_name"].getDataString());
	if (!info.vaild)
		info = textureManager.getTextureInfo("none");
	shared_ptr<Item> item = nullptr;
	if (slotName.size() > 5)
		item = itemAllocator.allocate(slotName.substr(5), dataset, to_string(i) + to_string(j), false);

	int maxItemsThisSlot = maxItemsPerSlot;
	if (item != nullptr)
		maxItemsThisSlot = item->getMaxItemsPerSlotCount();

	if (extraPushId != -1)
		imgui::PushID(extraPushId);
	imgui::PushID(i * 9 + j);
	bool mapMutated = false;
	if (imgui::ImageButton(info.getSprite(), Vector2f(32, 32), 3)) {
		if (cursorName != slotName) {
			mapMutated = true;
			vector<pair<string, Data>> newCursorData, newSlotData;
			for (auto i = dataset.getDatasets().begin(); i != dataset.getDatasets().end();) {
				auto& d = *i;
				if (d.first.substr(0, 2) == slotPrefix) {
					newCursorData.push_back(make_pair(d.first.substr(2), d.second));
					i = dataset.getDatasets().erase(i);
				}
				else
					i++;
			}
			for (auto i = localPlayer->getDataset().getDatasets().begin(); i != localPlayer->getDataset().getDatasets().end();) {
				auto& d = *i;
				if (d.first.substr(0, 7) == "cursor_") {
					newSlotData.push_back(make_pair(d.first.substr(7), d.second));
					i = localPlayer->getDataset().getDatasets().erase(i);
				}
				else
					i++;
			}
			for (auto& i : newCursorData)
				localPlayer->getDataset().getDatasets().insert(make_pair("cursor_" + i.first, i.second));
			for (auto& i : newSlotData)
				dataset.getDatasets().insert(make_pair(slotPrefix + i.first, i.second));
		}
		else {
			// Stack items
			int sum = slotCount + cursorCount;
			int slotCnt = sum;
			if (slotCnt > maxItemsThisSlot)
				slotCnt = maxItemsThisSlot;
			int curCnt = sum - slotCnt;
			dataset[to_string(i) + to_string(j) + "count"].setData(slotCnt);
			localPlayer->getDataset()["cursor_count"].setData(curCnt);
			if (curCnt == 0)
				localPlayer->getDataset()["cursor_item_name"].setData(""s);
		}
	}

	if (info.id != "none") {
		if (FloatRect(imgui::GetItemRectMin(), imgui::GetItemRectSize()).contains(Vector2f(logicIO.mousePos))) {
			if (logicIO.mouseState[Mouse::Right] == LogicIO::JustPressed) {
				if (cursorName == "") {
					// Spilt half
					int curCnt = slotCount / 2 + slotCount % 2;
					localPlayer->getDataset()["cursor_item_name"].setData(slotName);
					localPlayer->getDataset()["cursor_count"].setData(curCnt);
					if (slotCount - curCnt == 0)
						dataset[to_string(i) + to_string(j) + "item_name"].setData(""s);
					dataset[to_string(i) + to_string(j) + "count"].setData(slotCount - curCnt);
				}
				else {
					// TODO Intentory gestures
					// Place one
					if (slotName == "" || slotName == cursorName) {
						dataset[to_string(i) + to_string(j) + "item_name"].setData(cursorName);
						if (slotCount + 1 <= maxItemsThisSlot) {
							dataset[to_string(i) + to_string(j) + "count"].setData(slotCount + 1);
							localPlayer->getDataset()["cursor_count"].setData(cursorCount - 1);
							if (cursorCount - 1 == 0)
								localPlayer->getDataset()["cursor_item_name"].setData(""s);
						}
					}
				}
			}
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

		if (slotCount != 1) {
			ImVec2 pos = imgui::GetItemRectMin(); pos.x += 2.0;
			imgui::GetCurrentWindow()->DrawList->AddText(pos, ImU32(0xFFFFFFFF), StringParser::toString(slotCount).c_str());
		}
	}
	if (extraPushId != -1)
		imgui::PopID();
	imgui::PopID();
}


////////////////////////////////////////
void PlayerInventoryUI::runImGui() {

}

