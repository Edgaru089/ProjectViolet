#pragma once

#include "Main.hpp"
#include "PlayerInventory.hpp"
#include "TextureManager.hpp"
#include "TextSystem.hpp"
#include "PlayerEntity.hpp"


////////////////////////////////////////
PlayerInventory::PlayerInventory() {

}


////////////////////////////////////////
void PlayerInventory::updateLogic() {
	for (int i = 0; i <= 3; i++)
		for (int j = 0; j < 9; j++) {
			if (localPlayer->getDataset()[to_string(i) + to_string(j) + "item_name"].getDataString() == "")
				continue;
			const string& name = localPlayer->getDataset()[to_string(i) + to_string(j) + "item_name"];
			shared_ptr<Item> item = itemAllocator.allocate(name.substr(5), localPlayer->getDataset(), to_string(i) + to_string(j), i == 0 && j == cursorId);
			if (item != nullptr) {
				item->updateLogic();
			}
		}
}


////////////////////////////////////////
void PlayerInventory::runImGui() {
	imgui::PushStyleColor(ImGuiCol_Button, Color::Transparent);
	imgui::PushStyleColor(ImGuiCol_ButtonHovered, Color(255, 255, 255, 64));
	imgui::PushStyleColor(ImGuiCol_ButtonActive, Color(255, 255, 255, 48));
	imgui::PushStyleColor(ImGuiCol_WindowBg, Color(0, 0, 0, 128));
	imgui::SetNextWindowPos(
		ImVec2(imgui::GetIO().DisplaySize.x / 2.0f, imgui::GetIO().DisplaySize.y + 1),
		ImGuiCond_Always,
		ImVec2(0.5f, 1.0f));
	imgui::Begin("BottomInventory", nullptr,
				 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
	for (int j = 0; j < 9; j++) {
		if (j != 0)
			imgui::SameLine(0, 3);
		if (j == cursorId) {
			imgui::PushStyleColor(ImGuiCol_Border, Color(192, 192, 192));
			imgui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.5);
		}

		TextureInfo info;
		if (localPlayer->getDataset()["0" + to_string(j) + "item_name"].getDataString().substr(0, 5) == "item_")
			info = itemAllocator.allocate(localPlayer->getDataset()["0" + to_string(j) + "item_name"].getDataString().substr(5),
										  localPlayer->getDataset(),
										  "0" + to_string(j),
										  j == cursorId)->getTextureInfo();
		else
			info = textureManager.getTextureInfo(localPlayer->getDataset()["0" + to_string(j) + "item_name"].getDataString());
		if (!info.vaild)
			info = textureManager.getTextureInfo("none");

		if (imgui::ImageButton(info.getSprite(), Vector2f(32, 32), 3)) {
			cursorId = j;
			imgui::PushStyleColor(ImGuiCol_Border, Color(192, 192, 192));
			imgui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.5);
		}

		if (info.id != "none") {
			ImVec2 pos = imgui::GetItemRectMin(); pos.x += 2.0;
			if (localPlayer->getDataset()["0" + to_string(j) + "count"].getDataInt() != 1)
				imgui::GetCurrentWindow()->DrawList->AddText(pos, ImU32(0xFFFFFFFF), StringParser::toString(localPlayer->getDataset()["0" + to_string(j) + "count"].getDataInt()).c_str());
		}

		if (j == cursorId) {
			imgui::PopStyleColor();
			imgui::PopStyleVar();
		}
	}
	Vector2f pos = imgui::GetWindowPos(), size = imgui::GetWindowSize();
	imgui::End();

	imgui::SetNextWindowPos(
		ImVec2(imgui::GetIO().DisplaySize.x / 2.0f, imgui::GetIO().DisplaySize.y - size.y + 2),
		ImGuiCond_Always,
		ImVec2(0.5f, 1.0f));
	imgui::SetNextWindowSize(ImVec2(size.x, 0.0f), ImGuiCond_Always);
	imgui::Begin("BottomInventoryExtend", nullptr,
				 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);

	const string& name = localPlayer->getDataset()["0" + to_string(cursorId) + "item_name"].getDataString();
	if (name != "") {
		imgui::Text(text.get(name + ".name"));
		shared_ptr<Item> item = itemAllocator.allocate(name.substr(5), localPlayer->getDataset(), "0" + to_string(cursorId), true);
		if (item != nullptr)
			item->_pushExtraImguiItemsToDashboard();
	}

	// Player Healthz
	imgui::PushStyleColor(ImGuiCol_PlotHistogram, Color(160, 0, 0));
	imgui::ProgressBar((float)localPlayer->getHealth() / localPlayer->getMaxHealth(), ImVec2(-1, 16), ("Health: " + to_string(localPlayer->getHealth()) + " / " + to_string(localPlayer->getMaxHealth())).c_str());
	imgui::PopStyleColor();

	imgui::End();

	imgui::PopStyleColor(4);
}


