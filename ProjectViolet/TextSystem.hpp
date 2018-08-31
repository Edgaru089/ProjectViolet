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
			mlog << Log::Error << "[TextSystem] File Open Failed!" << dlog;
			return false;
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

	bool loadFromMemory(const void* data, Uint64 size) {
		if (data == nullptr || size == 0) {
			mlog << Log::Error << "[TextSystem] Memory Access Failed!" << dlog;
			return false;
		}
		string datastr((char*)data, size);
		istringstream is(datastr);
		if (!is.good()) {
			mlog << Log::Error << "[TextSystem] Memory StringStream Open Failed!" << dlog;
			return false;
		}
		// UTF-8 Signature (EF BB BF)
		if (is.get() == 0xEF)
			is.ignore(2);
		else
			is.unget();
		string str;
		langs.clear();
		while (!getline(is, str).eof()) {
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
	map<string, string> langs;
	string empty;

};

TextSystem texts;
