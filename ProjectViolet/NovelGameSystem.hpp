#pragma once

#include "Main.hpp"

class NovelGameParagraph {
public:
	friend class NovelGameSystem;

	const float dialogueHeight = 200.0f;
	const Time stringCharAppearTime = milliseconds(20);

	void loadFromScriptFile(const string& filename);
	void start();

	void click();
	void runUntilPause();

	void updateLogic();
	void runImGui();

private:
	string strid;

	bool running, valid = false, paused;
	bool wantUpdateClick;
	ImFont* font;

	Time stringCharLastAppearTime;

	int stringTruncatedCnt;
	String text, title;
	shared_ptr<Texture> speaker;

	int nextCommand;
	vector<pair<string, string>> commands;

};


class NovelGameSystem {
public:

	void preInitalaize();

	void loadScriptFromFile(const string& filename, const string& id);
	void start(const string& id);

	void updateLogic();
	void runImGui();

	// Empty string if none
	string getCurrentParagraphId();

private:

	ImFont * font;
	map<string, NovelGameParagraph> paragraphs;
	NovelGameParagraph* current;

};


NovelGameSystem novelGameSystem;
