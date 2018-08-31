#pragma once

#include <vector>
#include <fstream>
#include <map>

#include "LogSystem.hpp"

using namespace std;

class OptionFile {
public:

	struct Option {
		Option() {}
		Option(int line) :line(line) {}
		Option(int line, string key, string value) :line(line), key(key), value(value) {}

		int line;
		string key;
		string value;
	};

	bool loadFromFile(string filename) {
		fin.open(filename);
		if (fin.bad())
			return false;
		string str;
		int i = -1;
		while (!getline(fin, str).eof()) {
			i++;
			fileLines.push_back(str);
			if (str[0] == '#')
				continue;
			size_t pos = str.find('=');
			if (pos == str.npos)
				continue;
			string key, value;
			key = str.substr(0u, pos);
			value = str.substr(pos + 1);
			options.insert(pair<string, Option>(key, Option(i, key, value)));
		}
		fin.close();
		return true;
	}

	bool saveToFile(string filename) {
		fout.open(filename);
		for (const pair<string, Option>& i : options)
			fileLines[i.second.line] = StringParser::toStringF("%s=%s", i.first.c_str(), i.second.value.c_str());
		for (string& i : fileLines)
			fout << i << '\n';
		fout.flush();
		fout.close();
		return true;
	}

	string get(string key) {
		return options[key].value;
	}

	void change(string key, string value) {
		auto i = options.find(key);
		if (i == options.end()) {
			options.insert(pair<string, Option>(key, Option(fileLines.size(), key, value)));
			fileLines.push_back(StringParser::toStringF("%s=%s", key.c_str(), value.c_str()));
		}
		else {
			i->second.value = value;
			fileLines[i->second.line] = StringParser::toStringF("%s=%s", i->first.c_str(), i->second.value.c_str());
		}
	}


private:

	ifstream fin;
	ofstream fout;

	vector<string> fileLines;
	map<string, Option> options;

};

