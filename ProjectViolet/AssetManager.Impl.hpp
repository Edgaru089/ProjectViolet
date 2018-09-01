
#include "AssetManager.hpp"
#include "SHA-256.hpp"


////////////////////////////////////////
bool AssetManager::loadAssetPack(string filename) {
	mlog << "[AssetManager] Loading asset pack file: " << filename << dlog;

	ifstream fin(filename, ifstream::binary);
	if (fin.bad()) {
		mlog << Log::Error << "[AssetManager] File failed to open." << dlog;
		return false;
	}

	char c = fin.get();
	if (c == 'E') { // File header "Edgaru089 Package! 0=w=0"
		mlog << "[AssetManager] Edgaru089 Package! 0=w=0" << dlog;
		fin.ignore(23);
	}
	else
		fin.unget();

	Uint64 listSize = 0;
	for (int i = 0; i < 8; i++) {
		Uint8 c = fin.get();
		listSize |= (c << (i * 8));
	}
	mlog << "[AssetManager] Reading asset list with a size of " << listSize << "..." << dlog;
	string listStr;
	listStr.reserve(listSize);
	for (int i = 0; i < listSize; i++)
		listStr.push_back(fin.get());

	istringstream strin(listStr);
	string str;
	sha256Digest.clear();
	while (getline(strin, str)) {
		char type = str[0];
		if (type == '#') // Comment line
			continue;
		else if (type == '%') { // Extra info line
			string infoType, infoContent;
			size_t i = 1;
			// read info type
			while (!isblank(str[i]) && !iscntrl(str[i])) {
				infoType += str[i];
				i++;
			}
			// find next non-blank character
			while (str[i] == ' ' || str[i] == '\t')
				i++;
			// read info contents
			while (!isblank(str[i]) && !iscntrl(str[i])) {
				infoContent += str[i];
				i++;
			}
			if (infoType == "sha256") // save digest
				sha256Digest = infoContent;
		}
		else if (type == '$') { // Asset line
			string id, type, file;
			string offstr, lenstr;
			IntRect rect(0, 0, 0, 0);
			size_t i = 1;
			// Find first non-blank character
			while (str[i] == ' ' || str[i] == '\t')
				i++;
			// Read the asset type
			while (!isspace(str[i])) {
				type += str[i];
				i++;
			}
			// Find first non-blank character
			while (str[i] == ' ' || str[i] == '\t')
				i++;
			// Read the asset id
			while (!isspace(str[i])) {
				id += str[i];
				i++;
			}
			// Find first digit char
			while (!isdigit(str[i]))
				i++;
			// Read offset word
			while (isdigit(str[i])) {
				offstr += str[i];
				i++;
			}
			// Find next digit char
			while (!isdigit(str[i]))
				i++;
			// Read length word
			while (isdigit(str[i])) {
				lenstr += str[i];
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

			mlog << "[AssetManager] Loaded asset " << id << dlog;
			assets.insert(make_pair(id, Asset{ (Uint64)StringParser::toLongLong(offstr), (Uint64)StringParser::toLongLong(lenstr), id, type, rect }));
		}
	}

	mlog << "[AssetManager] Reading data..." << dlog;
	char* buf = new char[32 * 1024 * 1024];
	while (!fin.eof()) {
		fin.read(buf, 32 * 1024 * 1024);
		realData.reserve(realData.size() + fin.gcount());
		for (int i = 0; i < fin.gcount(); i++)
			realData.push_back(buf[i]);
	}
	delete[] buf;

	// Load Texture Assets
	for (auto& i : assets) {
		if (i.second.type == "TEXTURE")
			if (i.second.textureRect == IntRect(0, 0, 0, 0))
				textureManager.addImage(i.second.strid, Data{ realData.data() + i.second.offset, i.second.length });
			else
				textureManager.addImage(i.second.strid, Data{ realData.data() + i.second.offset, i.second.length }, i.second.textureRect);
	}

	if (sha256Digest != "") {
		mlog << "[AssetManager] Data has SHA-256 digest, verifying..." << dlog;
		string realDigest;
		if ((realDigest = sha256(realData)) == sha256Digest)
			mlog << "[AssetManager] SHA-256 digest test passed." << dlog;
		else {
			mlog << Log::FatalError << "[AssetManager] SHA-256 digest test failed!" << dlog;
			mlog << Log::FatalError << "[AssetManager]  Saved Digest: " << sha256Digest << dlog;
			mlog << Log::FatalError << "[AssetManager] Actual Digest: " << realDigest << dlog;
			mlog << Log::FatalError << "[AssetManager] Your data pack might be modified! Aborting!" << dlog;
			throw runtime_error("AssetManager: Data pack SHA-256 digest failed");
		}
	}

	mlog << "[AssetManager] Asset file loaded." << dlog;

	return true;

}


////////////////////////////////////////
AssetManager::Data AssetManager::getAssetData(string id) {
	auto p = assets.find(id);
	if (p != assets.end())
		return Data{ realData.data() + p->second.offset, p->second.length };
	else
		return Data{ nullptr, 0 };
}

