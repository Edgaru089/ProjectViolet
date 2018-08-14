#pragma once

#include <map>

#include "Main.hpp"


struct TextureInfo {
	TextureInfo() :vaild(false) {}
	TextureInfo(Texture* texture, IntRect rect, string id) :texture(texture), textureRect(rect), id(id), vaild(true) {}

	Sprite getSprite() {
		if (!vaild)
			return Sprite();
		Sprite sp(*texture);
		sp.setTextureRect(textureRect);
		return sp;
	}

	Texture* texture;
	IntRect textureRect;
	string id;
	bool vaild;
};


class TextureManager {
public:

	const unsigned int maxImageWidth = 256;

	~TextureManager() { if (texture != nullptr)delete texture; }

	// Image is copied
	void addImage(string id, Image& image);
	void addImage(string id, string filename);
	void addImage(string id, string filename, IntRect textureRect);

	void bindTexture();

	TextureInfo getTextureInfo(string id);

	void clear();

	// Texture pointer may change after every rebind
	Texture* getBindingTexture();

private:
	// cannot use shared_ptr because of sf::RenderStates requirements
	Texture * texture;

	vector<pair<string, Image>> images;
	map<string, IntRect> rects;

};



TextureManager textureManager;
