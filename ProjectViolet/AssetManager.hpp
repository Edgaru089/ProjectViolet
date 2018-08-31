#pragma once

#include "Main.hpp"
#include "TextureManager.hpp"


class AssetManager {
public:

	struct Data {
		const void* data;
		Uint64 size;

		// the template class Resource must have a member function defined like:
		// bool loadFromMemory(const void* data, Uint64/size_t size);
		template<typename Resource>
		bool load(Resource& res) {
			return res.loadFromMemory(data, size);
		}
	};

	struct Asset {
		Uint64 offset, length;
		string strid, type;
		IntRect textureRect;
	};

	bool loadAssetPack(string filename = "data.bin");

	Data getAssetData(string id);

	map<string, Asset>& getAssetMapper() { return assets; }

private:

	map<string, Asset> assets;

	vector<char> realData;
	string sha256Digest;

};

AssetManager assetManager;
