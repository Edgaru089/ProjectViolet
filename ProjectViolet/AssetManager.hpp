#pragma once

#include "Main.hpp"
#include "TextureManager.hpp"


class AssetManager {
public:

	struct Asset {
		Asset() :textureRect(0, 0, 0, 0) {}
		Asset(string id, string filename, string type) :
			strid(id), filename(filename), type(type), textureRect(0, 0, 0, 0) {}
		Asset(string id, string filename, string type, IntRect textureRect) :
			strid(id), filename(filename), type(type), textureRect(textureRect) {}

		string strid, filename, type;
		IntRect textureRect;
	};

	bool loadListFile(string filename = "assets.list");

	string getAssetFilename(string id);

	unordered_map<string, Asset>& getAssetMapper() { return assets; }

private:

	unordered_map<string, Asset> assets;

};

AssetManager assetManager;
