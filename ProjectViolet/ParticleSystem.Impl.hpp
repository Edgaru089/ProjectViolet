#pragma once

#include "ParticleSystem.hpp"


////////////////////////////////////////
Particle::Particle(TextureInfo texture, double sizeDivisor, Time timeToLive, Vector2d onScreenSize, double gravity, double airFriction) {
	this->wholeText = texture;
	this->size = onScreenSize;
	this->gravity = gravity;
	this->airFriction = airFriction;

	Vector2d size;
	double posX, posY;
	if (!isSame(sizeDivisor, 1.0)) {
		size = Vector2d(Vector2d(texture.textureRect.width, texture.textureRect.height) / sizeDivisor);
		posX = rand01() * (texture.textureRect.width - size.x) + texture.textureRect.left;
		posY = rand01() * (texture.textureRect.height - size.y) + texture.textureRect.top;
	}
	else {
		size = Vector2d(texture.textureRect.width, texture.textureRect.height);
		posX = texture.textureRect.left;
		posY = texture.textureRect.top;
	}

	textureSubRect = IntRect(posX, posY, size.x, size.y);

	this->timeToLive = timeToLive;
	livedTime = milliseconds(0);

	alive = true;
}


////////////////////////////////////////
void Particle::_updateLogic() {

	// Air friction
	if (airFriction != .0) {
		vecX = vecX * pow(airFriction, logicIO.deltaTime.asSeconds());
		vecY = vecY * pow(airFriction, logicIO.deltaTime.asSeconds());
	}

	livedTime += logicIO.deltaTime;
	if (livedTime > timeToLive)
		kill();
}


////////////////////////////////////////
void Particle::_getBoundingCollisionPoints(vector<Vector2d>& points) {
	points.clear();
	points.push_back(getCenterPos());
}


////////////////////////////////////////
void PartlcleSystem::updateLogic() {
	for (auto i = parts.begin(); i != parts.end();) {
		if (!i->isAlive())
			i = parts.erase(i);
		else {
			i->updateLogic();
			i++;
		}
	}
}


////////////////////////////////////////
void PartlcleSystem::getRenderList(VertexArray& verts) {
	for (auto e = parts.begin(); e != parts.end();) {
		if (!e->isAlive())
			e = parts.erase(e);
		else {
			// Borrow EntityManager's code
			if (e->getHitbox().intersects(renderIO.viewRectBlock)) {
				TextureInfo tex = e->getTextureInfo();
				Vector2d center = e->getPosition()*renderIO.gameScaleFactor;
				double width = e->getSize().x*renderIO.gameScaleFactor, height = e->getSize().y*renderIO.gameScaleFactor;

				Vector2i chunk = TerrainManager::convertWorldCoordToChunkId(Vector2i(e->getPosition().x, e->getPosition().y));
				Vector2i inCc = TerrainManager::convertWorldCoordToInChunkCoord(Vector2i(e->getPosition().x, e->getPosition().y));
				shared_ptr<Chunk> c = terrainManager.getChunk(chunk);

				Uint8 mask;
				if (c != nullptr)
					mask = 63 + 192.0*((double)c->lightLevel[inCc.x][inCc.y] / maxLightingLevel);
				else
					mask = 63;

				// Left-Top
				verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y - height),
									Color(mask, mask, mask),
									Vector2f(tex.textureRect.left, tex.textureRect.top)
				));
				// Right-Top
				verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - height),
									Color(mask, mask, mask),
									Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top)
				));
				// Left-Bottom
				verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y),
									Color(mask, mask, mask),
									Vector2f(tex.textureRect.left, tex.textureRect.top + tex.textureRect.height)
				));
				// Right-Top
				verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y - height),
									Color(mask, mask, mask),
									Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top)
				));
				// Left-Bottom
				verts.append(Vertex(Vector2f(center.x - width / 2.0, center.y),
									Color(mask, mask, mask),
									Vector2f(tex.textureRect.left, tex.textureRect.top + tex.textureRect.height)
				));
				// Right-Bottom
				verts.append(Vertex(Vector2f(center.x + width / 2.0, center.y),
									Color(mask, mask, mask),
									Vector2f(tex.textureRect.left + tex.textureRect.width, tex.textureRect.top + tex.textureRect.height)
				));
			}
			e++;
		}
	}
}


////////////////////////////////////////
void PartlcleSystem::emitSmoke(Vector2d position,
	double speed,
	double gravity,
	double airFriction,
	double angleBegin,
	double angleEnd,
	int count) {
	emitSmoke(DoubleRect(position.x, position.y, .0, .0), speed, gravity, airFriction, angleBegin, angleEnd, count);
}


////////////////////////////////////////
void PartlcleSystem::emitSmoke(DoubleRect position,
	double speed,
	double gravity,
	double airFriction,
	double angleBegin,
	double angleEnd,
	int count) {
	double sizeDivisor = 1.0;
	Time liveTimeBegin = seconds(2), liveTimeEnd = seconds(4);
	Vector2d size = Vector2d(0.4, 0.4);
	position.top += size.y / 2.0;

	if (liveTimeBegin > liveTimeEnd)
		swap(liveTimeBegin, liveTimeEnd);
	if (angleBegin > angleEnd)
		swap(angleBegin, angleEnd);
	for (int i = 0; i < count; i++) {
		TextureInfo texture = textureManager.getTextureInfo(StringParser::toStringF(
			"particle_smoke_%d",
			rand(1, 8)));

		Time live = liveTimeBegin + microseconds((liveTimeEnd - liveTimeBegin).asMicroseconds()*rand01());
		double angle = angleBegin + (angleEnd - angleBegin)*rand01();

		Particle p(texture, sizeDivisor, live, size, gravity, airFriction);
		p.setPosition(Vector2d(position.left + position.width*rand01(), position.top + position.height*rand01()));
		p.accelerateVector(speed * 1.2 * rand01(), angle);

		parts.push_back(p);
	}
}


////////////////////////////////////////
void PartlcleSystem::emitArrowGlow(Vector2d position) {
	emit(position, textureManager.getTextureInfo("particle_arrow_glow"),
		1.0, 0.0, 1, milliseconds(250), milliseconds(250), Vector2d(0.3, 0.3), 0.0);
}


////////////////////////////////////////
void PartlcleSystem::emit(Vector2d position, TextureInfo texture, double sizeDivisor, double speed, int count, Time liveTimeBegin, Time liveTimeEnd, Vector2d size, double gravity) {
	if (liveTimeBegin > liveTimeEnd)
		swap(liveTimeBegin, liveTimeEnd);
	position += Vector2d(.0, size.y / 2.0);
	for (int i = 0; i < count; i++) {
		Time live = liveTimeBegin + microseconds((liveTimeEnd - liveTimeBegin).asMicroseconds()*rand01());
		double angle = 360.0*rand01();

		Particle p(texture, sizeDivisor, live, size, gravity);
		p.setPosition(position);
		p.accelerateVector(speed * 1.2 * rand01(), angle);

		parts.push_back(p);
	}
}


////////////////////////////////////////
void PartlcleSystem::emit(DoubleRect range, TextureInfo texture, double sizeDivisor, double speed, int count, Time liveTimeBegin, Time liveTimeEnd, Vector2d size, double gravity) {
	if (liveTimeBegin > liveTimeEnd)
		swap(liveTimeBegin, liveTimeEnd);
	for (int i = 0; i < count; i++) {
		Time live = liveTimeBegin + microseconds((liveTimeEnd - liveTimeBegin).asMicroseconds()*rand01());
		double angle = 360.0*rand01();
		Vector2d pos;

		pos.x = range.left + range.width*rand01();
		pos.y = range.top + range.height*rand01();
		pos += Vector2d(.0, size.y / 2.0);

		Particle p(texture, sizeDivisor, live, size, gravity);
		p.setPosition(pos);
		p.accelerateVector(speed * 1.2 * rand01(), angle);

		parts.push_back(p);
	}
}


////////////////////////////////////////
void PartlcleSystem::emit(Vector2d position, TextureInfo texture, double sizeDivisor, double speed, int count, double angleBegin, double angleEnd, Time liveTimeBegin, Time liveTimeEnd, Vector2d size, double gravity) {
	if (liveTimeBegin > liveTimeEnd)
		swap(liveTimeBegin, liveTimeEnd);
	if (angleBegin > angleEnd)
		swap(angleBegin, angleEnd);
	position += Vector2d(.0, size.y / 2.0);
	for (int i = 0; i < count; i++) {
		Time live = liveTimeBegin + microseconds((liveTimeEnd - liveTimeBegin).asMicroseconds()*rand01());
		double angle = angleBegin + (angleEnd - angleBegin)*rand01();

		Particle p(texture, sizeDivisor, live, size, gravity);
		p.setPosition(position);
		p.accelerateVector(speed * 1.2 * rand01(), angle);

		parts.push_back(p);
	}
}


////////////////////////////////////////
void PartlcleSystem::emit(DoubleRect range, TextureInfo texture, double sizeDivisor, double speed, int count, double angleBegin, double angleEnd, Time liveTimeBegin, Time liveTimeEnd, Vector2d size, double gravity) {
	if (liveTimeBegin > liveTimeEnd)
		swap(liveTimeBegin, liveTimeEnd);
	if (angleBegin > angleEnd)
		swap(angleBegin, angleEnd);
	for (int i = 0; i < count; i++) {
		Time live = liveTimeBegin + microseconds((liveTimeEnd - liveTimeBegin).asMicroseconds()*rand01());
		double angle = angleBegin + (angleEnd - angleBegin)*rand01();
		Vector2d pos;

		pos.x = range.left + range.width*rand01();
		pos.y = range.top + range.height*rand01();
		pos += Vector2d(.0, size.y / 2.0);

		Particle p(texture, sizeDivisor, live, size, gravity);
		p.setPosition(pos);
		p.accelerateVector(speed * 1.2 * rand01(), angle);

		parts.push_back(p);
	}
}

