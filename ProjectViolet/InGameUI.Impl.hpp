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

	curUI->runImGui();

	imgui::SetNextWindowPos(
		ImVec2(imgui::GetIO().DisplaySize.x / 2.0f, imgui::GetIO().DisplaySize.y / 2.0f),
		ImGuiCond_Always,
		ImVec2(0.5f, 0.5f));
	if (curUI->showInventory()) {
		if (imgui::BeginPopupModal(curUI->windowTitle().c_str(), nullptr,
								   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove)) {

			_runInventoryUI();

			// Cursor item
			TextureInfo info = textureManager.getTextureInfo(playerInventory.slotCursor["item_name"].getDataString());
			if (info.vaild) {
				ImVec2 pos = imgui::GetIO().MousePos;
				imgui::GetOverlayDrawList()->AddImage((ImTextureID)info.texture->getNativeHandle(),
					ImVec2(pos.x - 16, pos.y - 16), ImVec2(pos.x + 16, pos.y + 16),
					ImVec2(info.textureRect.left / (float)info.texture->getSize().x,
						   info.textureRect.top / (float)info.texture->getSize().y),
					ImVec2((info.textureRect.left + info.textureRect.width) / (float)info.texture->getSize().x, (
						info.textureRect.top + info.textureRect.height) / (float)info.texture->getSize().y));
				pos.x -= 16 + 3 - 2; pos.y -= 16 + 3;
				if (playerInventory.slotCursor["count"].getDataInt() != 1)
					imgui::GetOverlayDrawList()->AddText(pos, ImU32(0xFFFFFFFF), StringParser::toString(playerInventory.slotCursor["count"].getDataInt()).c_str());
			}

			imgui::EndPopup();
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
			PlayerInventoryUI::ImGuiInventorySlot(playerInventory.slots[i][j], 1677216 + i * 9 + j);
		}
	}
	imgui::Dummy(ImVec2(.0f, 6.0f));
	for (int j = 0; j < 9; j++) {
		if (j != 0)
			imgui::SameLine();
		PlayerInventoryUI::ImGuiInventorySlot(playerInventory.slots[0][j], 1677216 + j);
	}
}


////////////////////////////////////////
void PlayerInventoryUI::ImGuiInventorySlot(Dataset& slotData, int pushId) {
	Dataset& cursor = playerInventory.slotCursor;

	string& slotName = slotData["item_name"].getDataString(), &cursorName = cursor["item_name"].getDataString();
	int& slotCount = slotData["count"].getDataInt(), &cursorCount = cursor["count"].getDataInt();

	TextureInfo info = textureManager.getTextureInfo(slotData["item_name"].getDataString());
	if (!info.vaild)
		info = textureManager.getTextureInfo("none");
	shared_ptr<Item> item = nullptr;
	if (slotName.size() > 5)
		item = itemAllocator.allocate(slotName.substr(5), slotData);

	int maxItemsThisSlot = maxItemsPerSlot;
	if (item != nullptr)
		maxItemsThisSlot = item->getMaxItemsPerSlotCount();

	if (pushId != -1)
		imgui::PushID(pushId);
	if (imgui::ImageButton(info.getSprite(), Vector2f(32, 32), 3)) {
		if (cursorName != slotName)
			swap(slotData, cursor);
		else {
			// Stack items
			int sum = slotCount + cursorCount;
			int slotCnt = sum;
			if (slotCnt > maxItemsThisSlot)
				slotCnt = maxItemsThisSlot;
			int curCnt = sum - slotCnt;
			slotCount = slotCnt;
			cursorCount = curCnt;
			if (curCnt == 0)
				cursorName = ""s;
		}
	}

	if (info.id != "none") {
		if (FloatRect(imgui::GetItemRectMin(), imgui::GetItemRectSize()).contains(Vector2f(logicIO.mousePos))) {
			if (logicIO.mouseState[Mouse::Right] == LogicIO::JustPressed) {
				if (cursorName == "") {
					// Spilt half
					int curCnt = slotCount / 2 + slotCount % 2;
					cursorName = slotName;
					cursorCount = curCnt;
					if (slotCount - curCnt == 0)
						slotName = ""s;
					slotCount = slotCount - curCnt;
				}
				else {
					// TODO Intentory gestures
					// Place one
					if (slotName == "" || slotName == cursorName) {
						slotName = cursorName;
						if (slotCount + 1 <= maxItemsThisSlot) {
							slotCount++;
							cursorCount--;
							if (cursorCount == 0)
								cursorName = "";
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
	if (pushId != -1)
		imgui::PopID();
}


////////////////////////////////////////
void PlayerInventoryUI::runImGui() {

}

