
#include "AssetManager.hpp"


////////////////////////////////////////
bool AssetManager::loadListFile(string filename) {
	mlog << "[AssetManager] Loading asset list file: " << filename << dlog;

	ifstream fin(filename);
	if (fin.bad()) {
		mlog << Log::Error << "[AssetManager] File failed to open." << dlog;
		return false;
	}

	string str;
	string assetType;

	while (getline(fin, str)) {
		char type = str[0];
		if (type == '#') // Comment line
			continue;
		else if (type == '[') { // Set asset type
			size_t end = str.find_first_of(']');
			assetType = str.substr(1, end - 1);
		}
		else if (type == '$') { // Asset line
			string id, file;
			IntRect rect(0, 0, 0, 0);
			size_t i = 1;

			// Find first non-blank character
			while (str[i] == ' ' || str[i] == '\t')
				i++;

			// Read the asset id
			while (!isspace(str[i])) {
				id += str[i];
				i++;
			}

			// Find first non-blank character
			while (str[i] == ' ' || str[i] == '\t')
				i++;

			// Read the filename
			while (i < str.size() && str[i] != '[' && !isspace(str[i])) {
				file += str[i];
				i++;
			}

			// Find texure rect info if possible
			while (i < str.size() && str[i] != '[')
				i++;
			// Has texture rect infomation - read it
			if (i < str.size() && str[i] == '[') {
				string left, top, width, height;
				// Every value is not negative
				// Read left
				while (!isdigit(str[i]))
					i++;
				while (isdigit(str[i])) {
					left += str[i];
					i++;
				}
				//Read top
				while (!isdigit(str[i]))
					i++;
				while (isdigit(str[i])) {
					top += str[i];
					i++;
				}
				//Read width
				while (!isdigit(str[i]))
					i++;
				while (isdigit(str[i])) {
					width += str[i];
					i++;
				}
				//Read height
				while (!isdigit(str[i]))
					i++;
				while (isdigit(str[i])) {
					height += str[i];
					i++;
				}
				rect = IntRect(StringParser::toInt(left), StringParser::toInt(top),
							   StringParser::toInt(width), StringParser::toInt(height));
			}

			mlog << "[AssetManager] Loaded " << assetType << " asset " << id << dlog;
			assets.insert(make_pair(id, Asset(id, file, assetType, rect)));
		}
	}

	// Load Texture Assets
	for (auto& i : assets) {
		if (i.second.type == "TEXTURE")
			if (i.second.textureRect == IntRect(0, 0, 0, 0))
				textureManager.addImage(i.second.strid, i.second.filename);
			else
				textureManager.addImage(i.second.strid, i.second.filename, i.second.textureRect);
	}


	mlog << "[AssetManager] Asset file loaded." << dlog;

	return true;

}


////////////////////////////////////////
string AssetManager::getAssetFilename(string id) {
	auto p = assets.find(id);
	if (p != assets.end())
		return p->second.filename;
	else
		return "";
}

