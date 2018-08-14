#pragma once

#include <fstream>
#include <vector>
#include "StringParser.hpp"

using namespace std;


struct OptionItem {

	OptionItem() {}
	OptionItem(string type, string name, string contents) :
		type(type), name(name), contents(contents) {}

	string type;
	string name;
	string contents;
};

class OptionFileAdv {



};
