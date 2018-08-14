#pragma once

#include "GamePausedUI.hpp"
#include "TextSystem.hpp"


////////////////////////////////////////
string GamePausedUI::windowTitle() {
	return text.getstr("menu.paused");
}


////////////////////////////////////////
void GamePausedUI::runImGui() {
	imgui::PushStyleColor(ImGuiCol_PopupBg, Color(0, 0, 0, 0));
	imgui::PushStyleColor(ImGuiCol_TitleBgActive, imgui::GetStyleColorVec4(ImGuiCol_PopupBg));
	imgui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, .0f);
	imgui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(.0f, 8.0f));
	imgui::SetNextWindowPos(
		ImVec2(imgui::GetIO().DisplaySize.x / 2.0f, imgui::GetIO().DisplaySize.y / 2.0f),
		ImGuiCond_Always,
		ImVec2(0.5f, 0.9f));
	if (imgui::BeginPopupModal(windowTitle().c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove)) {
		imgui::PushStyleColor(ImGuiCol_Button, Color(0, 0, 0, 192));
		imgui::PushStyleColor(ImGuiCol_ButtonHovered, Color(64, 64, 64, 192));
		imgui::PushStyleColor(ImGuiCol_ButtonActive, Color(48, 48, 48, 192));
		imgui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		imgui::Dummy(ImVec2(.0f, 5.0f));
		if (imgui::Button(text.get("menu.resume"), ImVec2(300, 36)))
			uiManager.changeUI(nullptr);
		if (imgui::Button(text.get("menu.options"), ImVec2(300, 36)))
			; // TODO Options
		if (imgui::Button(text.get("menu.backtotitle"), ImVec2(300, 36)))
			; // TODO Title Screen
		imgui::PopStyleVar();
		imgui::PopStyleColor(3);
		imgui::EndPopup();
	}
	imgui::PopStyleVar(2);
	imgui::PopStyleColor(2);
}

