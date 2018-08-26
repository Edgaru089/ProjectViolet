
#include "TextureManager.hpp"


////////////////////////////////////////
void TextureManager::addImage(string id, Image & image) {
	images.push_back(make_pair(id, image));
}


////////////////////////////////////////
void TextureManager::addImage(string id, AssetManager::Data data) {
	images.push_back(make_pair(id, Image()));
	images.back().second.loadFromMemory(data.data, data.size);
}


////////////////////////////////////////
void TextureManager::addImage(string id, AssetManager::Data data, IntRect textureRect) {
	Image tmp; // TODO FIXME Speed this up!
	tmp.loadFromMemory(data.data, data.size);
	images.push_back(make_pair(id, Image()));
	images.back().second.create(textureRect.width, textureRect.height);
	for (int i = 0; i < textureRect.width; i++)
		for (int j = 0; j < textureRect.height; j++)
			images.back().second.setPixel(i, j, tmp.getPixel(i + textureRect.left, j + textureRect.top));
}


////////////////////////////////////////
void TextureManager::bindTexture() {
	mlog << "[TextureManager] Binding texture..." << dlog;

	if (texture != nullptr)
		clear();

	int offX, offY, sizeX, sizeY;
	int maxHeight;
	offX = offY = sizeX = sizeY = maxHeight = 0;

	// HACK Add a dummy solid image to prevent skipping LightMask rendering
	// because of a transparent pixel
	Image dummy;
	dummy.create(1, 1);
	dummy.setPixel(0, 0, Color::Black);
	images.push_back(make_pair("dummy_transparent", dummy));

	sort(images.begin(), images.end(), [](const pair<string, Image>& x, const pair<string, Image>& y) {
		if (x.first == "dummy_transparent")
			return true;
		else if (y.first == "dummy_transparent")
			return false;
		else if (x.second.getSize().y < y.second.getSize().y)
			return true;
		else if (x.second.getSize().y == y.second.getSize().y&&x.second.getSize().x < y.second.getSize().x)
			return true;
		else
			return false;
	});

	for (auto& i : images) {

		// New line
		if (offX + i.second.getSize().x > maxImageWidth) {
			offX = 0;
			offY += maxHeight;
			maxHeight = 0;
		}

		rects.insert(make_pair(i.first, IntRect(offX, offY, i.second.getSize().x, i.second.getSize().y)));

		sizeX = max(sizeX, (int)(offX + i.second.getSize().x));
		sizeY = max(sizeY, (int)(offY + i.second.getSize().y));
		maxHeight = max(maxHeight, (int)(i.second.getSize().y));

		//mlog << "                 Texture Id: " << i.first << ", Pos(" <<
		//	offX << ", " << offY << "), Size: (" << i.second.getSize().x << ", " <<
		//	i.second.getSize().y << ")" << dlog;

		offX += i.second.getSize().x;
	}

	Image* image = new Image();
	image->create(sizeX, sizeY, Color::Transparent);

	for (auto& i : images) {
		const IntRect& rect = rects[i.first];
		for (int x = 0; x < rect.width; x++) {
			for (int y = 0; y < rect.height; y++) {
				image->setPixel(x + rect.left, y + rect.top, i.second.getPixel(x, y));
			}
		}
	}

	texture = new Texture();
	texture->loadFromImage(*image);
	delete image;

	mlog << "[TextureManager] Binding done." << dlog;
}


////////////////////////////////////////
TextureInfo TextureManager::getTextureInfo(string id) {
	if (texture == nullptr)
		return TextureInfo();
	else {
		auto i = rects.find(id);
		if (i != rects.end())
			return TextureInfo(texture, i->second, id);
		else
			return TextureInfo();
	}
}


////////////////////////////////////////
void TextureManager::clear() {
	if (texture != nullptr)
		delete texture;
	texture = nullptr;
	images.clear();
	rects.clear();
}


////////////////////////////////////////
Texture* TextureManager::getBindingTexture() {
	return texture;
}

