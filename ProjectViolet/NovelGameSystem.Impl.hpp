#pragma once

#include "NovelGameSystem.hpp"
#include "IO.hpp"
#include "AssetManager.hpp"


////////////////////////////////////////
void NovelGameParagraph::_loadFromIStream(istream& in) {
	valid = false;
	// UTF-8 Signature (EF BB BF)
	if (in.get() == 0xEF)
		in.ignore(2);
	else
		in.unget();
	string str;
	while (!getline(in, str).eof()) {
		if (in.bad()) {
			mlog << Log::Error << "[NovelGameParagraph] File Failed: Bad IO stream" << dlog;
			return;
		}
		if (str == "")
			continue;
		size_t pos = str.find('#');
		if (pos == string::npos)
			commands.push_back(make_pair(str, ""));
		else {
			string param = str.substr(pos + 1);
			if (param[param.size() - 1] == '\r')
				param.pop_back();
			commands.push_back(make_pair(str.substr(0, pos), param));
		}
	}
	valid = true;
}


////////////////////////////////////////
void NovelGameParagraph::loadFromScriptFile(const string& filename) {
	valid = false;
	ifstream fin(filename);
	if (fin.bad()) {
		mlog << Log::Error << "[NovelGameParagraph] File \"" << filename << "\" open failed" << dlog;
		return;
	}
	_loadFromIStream(fin);
}


////////////////////////////////////////
void NovelGameParagraph::loadFromScriptInMemory(const void* data, Uint64 size) {
	valid = false;
	istringstream strin(string((const char*)data, size));
	_loadFromIStream(strin);
	valid = true;
}


////////////////////////////////////////
void NovelGameParagraph::start() {
	if (!valid)
		return;
	running = true;

	nextCommand = 0;
	wantUpdateClick = false;
	runUntilPause();
}


////////////////////////////////////////
void NovelGameParagraph::click() { wantUpdateClick = true; }


////////////////////////////////////////
void NovelGameParagraph::runUntilPause() {
	while (running) {
		if (nextCommand >= commands.size()) {
			running = false;
			break;
		}

		auto comm = toupper(commands[nextCommand].first);
		auto& param = commands[nextCommand].second;
		if (comm == "TEXT" || comm == "TX") {
			text = u8ToSfString(StringParser::replaceSubString(param, { { "\\n", "\n" } }));
			stringCharLastAppearTime = milliseconds(0);
			stringTruncatedCnt = 0;
		}
		else if (comm == "TEXTPAUSE" || comm == "TP" || comm == "") {
			text = u8ToSfString(StringParser::replaceSubString(param, { { "\\n", "\n" } }));
			stringCharLastAppearTime = milliseconds(0);
			stringTruncatedCnt = 0;
			nextCommand++;
			break;
		}
		else if (comm == "TITLE" || comm == "TI")
			title = u8ToSfString(param);
		else if (comm == "SPEAKER" || comm == "SP" || comm == "IMAGE" || comm == "IM") {
			if (param == "")
				speaker = nullptr;
			else {
				if (speaker == nullptr)
					speaker = make_shared<Texture>();
				assetManager.getAssetData(param).load(*speaker);
			}
		}
		else if (comm == "SPEAKERFILE" || comm == "IMAGEFILE") {
			if (param == "")
				speaker = nullptr;
			else {
				if (speaker == nullptr)
					speaker = make_shared<Texture>();
				speaker->loadFromFile(param);
			}
		}
		else if (comm == "PAUSE") {
			nextCommand++;
			break;
		}

		nextCommand++;
	}
}


////////////////////////////////////////
void NovelGameParagraph::updateLogic() {
	if (!running)
		return;

	if (wantUpdateClick) {
		wantUpdateClick = false;
		runUntilPause();
		stringCharLastAppearTime = milliseconds(0);
		stringTruncatedCnt = 0;
	}

	stringCharLastAppearTime += logicIO.deltaTime;
	if (stringTruncatedCnt < text.getSize() &&
		stringCharLastAppearTime >= stringCharAppearTime) {
		stringCharLastAppearTime = milliseconds(0);
		stringTruncatedCnt++;
	}
}


////////////////////////////////////////
void NovelGameParagraph::runImGui() {
	if (!running)
		return;
	if (font != nullptr)
		imgui::PushFont(font);
	imgui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
	imgui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
	imgui::PushStyleColor(ImGuiCol_WindowBg, Color(0, 0, 0, 128));

	imgui::SetNextWindowPos(ImVec2(imgui::GetIO().DisplaySize.x / 2.0f, imgui::GetIO().DisplaySize.y - 10.0f),
							ImGuiCond_Always, ImVec2(.5f, 1.0f));
	imgui::SetNextWindowSize(ImVec2(imgui::GetIO().DisplaySize.x - 20.0f, dialogueHeight), ImGuiCond_Always);
	imgui::Begin("##NovelgameMain", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);

	imgui::TextUnformatted(reinterpret_cast<const char*>(text.substring(0, stringTruncatedCnt).toUtf8().c_str()));

	if (imgui::IsWindowHovered() && logicIO.mouseState[Mouse::Left] == LogicIO::JustPressed) {
		click();
	}

	imgui::End();

	if (title != "") {
		imgui::PopStyleVar();
		imgui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
		imgui::SetNextWindowPos(ImVec2(10.0f, imgui::GetIO().DisplaySize.y - 10.0f - dialogueHeight - 5.0f),
								ImGuiCond_Always, ImVec2(.0f, 1.0f));
		imgui::Begin("##NovelgameTitle", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
		imgui::TextUnformatted(reinterpret_cast<const char*>(title.toUtf8().c_str()));
		imgui::End();
	}

	imgui::PopStyleVar(2);
	imgui::PopStyleColor();

	if (speaker != nullptr) {
		ImVec2 size = imgui::GetIO().DisplaySize;
		imgui::PushStyleVar(ImGuiStyleVar_WindowRounding, .0f);
		imgui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, .0f);
		imgui::SetNextWindowBgAlpha(.0f);
		imgui::SetNextWindowPos(size, ImGuiCond_Always, ImVec2(1.0f, 1.0f));
		imgui::SetNextWindowSize(speaker->getSize());
		imgui::Begin("##SpeakerOverlay", nullptr,
					 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoResize);
		imgui::GetWindowDrawList()->AddImage((ImTextureID)speaker->getNativeHandle(),
											 ImVec2(size.x - speaker->getSize().x, size.y - speaker->getSize().y),
											 size);
		imgui::End();
		imgui::PopStyleVar(2);
	}
	if (font != nullptr)
		imgui::PopFont();
}


////////////////////////////////////////
void NovelGameSystem::preInitalaize() {
	mlog << "[NovelGameSystem] Loading Novelgame Font..." << dlog;

	ImFontConfig config;
	strcpy(config.Name, "novelgame_font");
	config.RasterizerMultiply = 1.2f;
	config.OversampleH = 3;
	config.OversampleV = 1.5;

	AssetManager::Data d = assetManager.getAssetData("novelgame_font");
	font = imgui::GetIO().Fonts->AddFontFromMemoryTTF(
		const_cast<void*>(d.data), d.size,
		/*"simsun.ttc",*/
		28.0f,
		&config,
		getGlyphRangesCustomChinese()
	);

	//font = imgui::GetIO().Fonts->AddFontFromFileTTF(
	//	assetManager.getAssetFilename("novelgame_font").c_str(),
	//	28.0f,
	//	&config,
	//	imgui::GetIO().Fonts->GetGlyphRangesChineseSimplifiedCommon()
	//);

	mlog << "[NovelGameSystem] Loading Novelgame Scripts..." << dlog;
	for (auto& i : assetManager.getAssetMapper()) {
		if (i.second.type == "NOVELGAME_SCRIPT") {
			const string& id = i.second.strid;
			AssetManager::Data d = assetManager.getAssetData(id);
			size_t pos = id.find('_');
			if (pos != string::npos)
				loadScriptFromMemory(d.data, d.size, id.substr(pos + 1));
			else
				loadScriptFromMemory(d.data, d.size, id);
		}
	}
}


////////////////////////////////////////
void NovelGameSystem::loadScriptFromFile(const string& filename, const string& id) {
	mlog << "[NovelGameSystem] Loading paragraph \"" << id << "\" from file \"" << filename << "\"" << dlog;
	if (paragraphs.find(id) != paragraphs.end())
		mlog << Log::Warning << "[NovelGameSystem] Warning: Paragraph with id \"" << id << "\" already exists, skipping" << dlog;
	auto i = paragraphs.insert(make_pair(id, NovelGameParagraph())).first;
	auto& para = i->second;
	para.loadFromScriptFile(filename);
	if (!para.valid) {
		mlog << Log::Error << "[NovelGameSystem] Paragraph returned error upon loading" << dlog;
		paragraphs.erase(i);
	}
	para.strid = id;
	para.font = font;
	mlog << "[NovelGameSystem] Loaded " << para.commands.size() << " commands" << dlog;
}


////////////////////////////////////////
void NovelGameSystem::loadScriptFromMemory(const void* data, Uint64 size, const string& id) {
	mlog << "[NovelGameSystem] Loading paragraph \"" << id << "\" from memory" << dlog;
	if (paragraphs.find(id) != paragraphs.end())
		mlog << Log::Warning << "[NovelGameSystem] Warning: Paragraph with id \"" << id << "\" already exists, skipping" << dlog;
	auto i = paragraphs.insert(make_pair(id, NovelGameParagraph())).first;
	auto& para = i->second;
	para.loadFromScriptInMemory(data, size);
	if (!para.valid) {
		mlog << Log::Error << "[NovelGameSystem] Paragraph returned error upon loading" << dlog;
		paragraphs.erase(i);
	}
	para.strid = id;
	para.font = font;
	mlog << "[NovelGameSystem] Loaded " << para.commands.size() << " commands" << dlog;
}


////////////////////////////////////////
void NovelGameSystem::start(const string& id) {
	auto i = paragraphs.find(id);
	if (i == paragraphs.end()) {
		mlog << Log::Error << "[NovelGameSystem] When starting: Paragraph with id \"" << id << "\" not found" << dlog;
		return;
	}
	i->second.start();
	current = &(i->second);
}


////////////////////////////////////////
void NovelGameSystem::updateLogic() {
	if (current != nullptr) {
		current->updateLogic();
		if (!current->running)
			current = nullptr;
	}
}


////////////////////////////////////////
void NovelGameSystem::runImGui() {
	if (current != nullptr)
		current->runImGui();
}


////////////////////////////////////////
string NovelGameSystem::getCurrentParagraphId() {
	if (current == nullptr)
		return "";
	else
		return current->strid;
}

