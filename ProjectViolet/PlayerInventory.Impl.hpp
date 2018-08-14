#pragma once

#include "Main.hpp"
#include "PlayerInventory.hpp"
#include "TextureManager.hpp"
#include "TextSystem.hpp"
#include "PlayerEntity.hpp"


////////////////////////////////////////
PlayerInventory::PlayerInventory() {
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 9; j++) {
			_emptyDatasetSlot(slots[i][j]);
		}
	_emptyDatasetSlot(slotCursor);
	slots[0][0].setData("item_name", "item_bow"s);
	slots[0][0].setData("count", 1);
	slots[0][1].setData("item_name", "item_minigun"s);
	slots[0][1].setData("count", 1);
	slots[0][2].setData("item_name", "item_rpg_launcher"s);
	slots[0][2].setData("count", 1);
	slots[0][3].setData("item_name", "item_m16a4"s);
	slots[0][3].setData("count", 1);
	slots[0][4].setData("item_name", "item_barrett"s);
	slots[0][4].setData("count", 1);
	slots[0][5].setData("item_name", "item_emx3"s);
	slots[0][5].setData("count", 1);
	slots[0][6].setData("item_name", "item_vtx65"s);
	slots[0][6].setData("count", 1);
	slots[0][7].setData("item_name", "item_mrm36"s);
	slots[0][7].setData("count", 1);
	slots[0][8].setData("item_name", "item_sg55"s);
	slots[0][8].setData("count", 1);

	slots[1][0].setData("item_name", "item_minigun_ammo"s);
	slots[1][0].setData("count", 8);
	slots[1][1].setData("item_name", "item_rpg_ammo"s);
	slots[1][1].setData("count", 16);
	slots[1][2].setData("item_name", "item_arrow"s);
	slots[1][2].setData("count", 64);
	slots[1][3].setData("item_name", "item_m16a4_ammo"s);
	slots[1][3].setData("count", 12);
	slots[1][4].setData("item_name", "item_barrett_ammo"s);
	slots[1][4].setData("count", 6);
	slots[1][5].setData("item_name", "item_emx3_ammo"s);
	slots[1][5].setData("count", 6);
	slots[1][6].setData("item_name", "item_vtx65_ammo"s);
	slots[1][6].setData("count", 6);
	slots[1][7].setData("item_name", "item_mrm36_ammo"s);
	slots[1][7].setData("count", 4);
	slots[1][8].setData("item_name", "item_sg55_ammo"s);
	slots[1][8].setData("count", 8);

	slots[2][0].setData("item_name", "item_grenade"s);
	slots[2][0].setData("count", 8);
	slots[2][1].setData("item_name", "item_grenade"s);
	slots[2][1].setData("count", 8);
	slots[2][2].setData("item_name", "item_grenade"s);
	slots[2][2].setData("count", 8);
	slots[2][3].setData("item_name", "item_grenade"s);
	slots[2][3].setData("count", 8);
	slots[2][4].setData("item_name", "item_grenade"s);
	slots[2][4].setData("count", 8);
	slots[2][5].setData("item_name", "item_grenade"s);
	slots[2][5].setData("count", 8);
	slots[2][6].setData("item_name", "item_grenade"s);
	slots[2][6].setData("count", 8);
	slots[2][7].setData("item_name", "item_grenade"s);
	slots[2][7].setData("count", 8);
	slots[2][8].setData("item_name", "item_grenade"s);
	slots[2][8].setData("count", 8);

	slots[3][0].setData("item_name", "block_ladder"s);
	slots[3][0].setData("count", 5);
	slots[3][1].setData("item_name", "block_torch"s);
	slots[3][1].setData("count", 1);
}


////////////////////////////////////////
void PlayerInventory::updateLogic() {
	for (int i = 0; i <= 3; i++)
		for (int j = 0; j < 9; j++) {
			if (slots[i][j]["item_name"].getDataString() == "")
				continue;
			const string& name = slots[i][j]["item_name"];
			shared_ptr<Item> item = itemAllocator.allocate(name.substr(5), slots[i][j], i == 0 && j == cursorId);
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
		if (slots[0][j]["item_name"].getDataString().substr(0, 5) == "item_")
			info = itemAllocator.allocate(slots[0][j]["item_name"].getDataString().substr(5),
										  slots[0][j],
										  j == cursorId)->getTextureInfo();
		else
			info = textureManager.getTextureInfo(slots[0][j]["item_name"].getDataString());
		if (!info.vaild)
			info = textureManager.getTextureInfo("none");

		if (imgui::ImageButton(info.getSprite(), Vector2f(32, 32), 3)) {
			cursorId = j;
			imgui::PushStyleColor(ImGuiCol_Border, Color(192, 192, 192));
			imgui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.5);
		}

		if (info.id != "none") {
			ImVec2 pos = imgui::GetItemRectMin(); pos.x += 2.0;
			if (slots[0][j]["count"].getDataInt() != 1)
				imgui::GetCurrentWindow()->DrawList->AddText(pos, ImU32(0xFFFFFFFF), StringParser::toString(slots[0][j]["count"].getDataInt()).c_str());
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

	const string& name = slots[0][cursorId]["item_name"].getDataString();
	if (name != "") {
		imgui::Text(text.get(name + ".name"));
		shared_ptr<Item> item = itemAllocator.allocate(name.substr(5), slots[0][cursorId], true);
		if (item != nullptr)
			item->_pushExtraImguiItemsToDashboard();
	}

	// Player Health
	imgui::PushStyleColor(ImGuiCol_PlotHistogram, Color(160, 0, 0));
	imgui::ProgressBar((float)localPlayer->getHealth() / localPlayer->getMaxHealth(), ImVec2(-1, 16), ("Health: " + to_string(localPlayer->getHealth()) + " / " + to_string(localPlayer->getMaxHealth())).c_str());
	imgui::PopStyleColor();

	imgui::End();

	imgui::PopStyleColor(4);
}


////////////////////////////////////////
void PlayerInventory::_emptyDatasetSlot(Dataset& slot) {
	slot.getDatasets().clear();
	slot.setData("item_name", ""s);
	slot.setData("count", 0);
}

