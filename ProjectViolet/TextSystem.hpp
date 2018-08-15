#pragma once

#include <fstream>
#include <string>
#include <map>
#include <SFML/System.hpp>

#include "LogSystem.hpp"
#include "Main.hpp"
#include "Lockable.hpp"

using namespace std;
using namespace sf;

class TextSystem :public Lockable {
public:

	bool loadFromFile(const string& filename) {
		AUTOLOCKABLE(*this);
		mlog << "[TextSystem] Loading langfile:" << filename << dlog;
		ifstream fin(filename);
		if (!fin.good()) {
			return false;
			mlog << Log::Error << "             File Open Failed!" << dlog;
		}

		// UTF-8 Signature (EF BB BF)
		if (fin.get() == 0xEF)
			fin.ignore(2);
		else
			fin.unget();

		string str;
		langs.clear();
		while (!getline(fin, str).eof()) {
			size_t pos;
			if (str.size() == 0 || str[0] == '#' || (pos = str.find('=')) == string::npos)
				continue;
			string id = str.substr(0, pos), cont = str.substr(pos + 1);
			//mlog << "             Loaded object: " << id << " = " << cont << dlog;
			langs.insert(make_pair(id, StringParser::replaceSubString(cont, { { "\\n", "\n" } })));
		}

		return true;
	}

	const char* get(string id) {
		AUTOLOCKABLE(*this);
		auto i = langs.find(id);
		if (i == langs.end())
			return empty.c_str();
		else
			return i->second.c_str();
	}

	const string& getstr(string id) {
		AUTOLOCKABLE(*this);
		auto i = langs.find(id);
		if (i == langs.end())
			return empty;
		else
			return i->second;
	}

	String getSfString(string id) {
		AUTOLOCKABLE(*this);
		auto i = langs.find(id);
		if (i == langs.end())
			return String();
		else
			return String::fromUtf8(i->second.begin(), i->second.end());
	}

private:
	// Id - TextUtf8
	unordered_map<string, string> langs;
	string empty;

};

TextSystem text;
