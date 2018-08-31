#pragma once

#include "MobPathFinder.hpp"
#include "TerrainManager.hpp"


////////////////////////////////////////
MobPath MobPathFinder::findPath(Vector2i beginPos, Vector2i targetPos, Vector2d hitboxSize, bool canClimbLadders, int maxSteps) {

	// Use raw BFS as search engine
	queue<pair<Vector2i, vector<MobPath::Action>>> Q;
	Q.push(make_pair(beginPos, vector<MobPath::Action>()));
	// Mark been points
	set<Vector2i, Vector2Less<int>> beenPoints;
	beenPoints.insert(beginPos);
	Vector2i pos;

	auto checkNotBeen = [&](Vector2i offset) {
		return beenPoints.find(pos + offset) == beenPoints.end();
	};
	auto checkBlockSolid = [&](Vector2i pos) {
		shared_ptr<Block> b = terrainManager.getBlock(pos);
		return b != nullptr && (b->isSolid() || (canClimbLadders && b->getBlockId() == "ladder"));
	};

	// Begin the game loop
	while (!Q.empty()) {
		auto now = Q.front(); Q.pop();
		pos = now.first;
		// Reached target - return
		if (pos == targetPos)
			return MobPath{ beginPos, targetPos, now.second };
		// Too many steps - continue
		if (now.second.size() + 1 > maxSteps)
			continue;

		// TODO Ladders
		if (canClimbLadders&&checkBlockId(pos, "ladder")) {
			// Ascend
			if (checkNotBeen(Vector2i(0, -1)) && checkHitboxOk(pos + Vector2i(0, -1), hitboxSize)) {
				beenPoints.insert(pos + Vector2i(0, -1));
				now.second.push_back(MobPath::AscendLadder);
				Q.push(make_pair(pos + Vector2i(0, -1), now.second));
				now.second.pop_back();
			}
			// Descend
			if (checkNotBeen(Vector2i(0, 1))) {
				beenPoints.insert(pos + Vector2i(0, 1));
				now.second.push_back(MobPath::DescendLadder);
				Q.push(make_pair(pos + Vector2i(0, 1), now.second));
				now.second.pop_back();
			}
		}

		if (checkBlockSolid(pos + Vector2i(0, 1)) || now.second.back() == MobPath::Jump || now.second.back() == MobPath::AscendLadder) {
			// Move left
			if (checkNotBeen(Vector2i(-1, 0)) && checkHitboxOk(pos + Vector2i(-1, 0), hitboxSize)) {
				beenPoints.insert(pos + Vector2i(-1, 0));
				now.second.push_back(MobPath::MoveLeft);
				Q.push(make_pair(pos + Vector2i(-1, 0), now.second));
				now.second.pop_back();
			}

			// Move right
			if (checkNotBeen(Vector2i(1, 0)) && checkHitboxOk(pos + Vector2i(1, 0), hitboxSize)) {
				beenPoints.insert(pos + Vector2i(1, 0));
				now.second.push_back(MobPath::MoveRight);
				Q.push(make_pair(pos + Vector2i(1, 0), now.second));
				now.second.pop_back();
			}

			// Jump
			if (now.second.back() != MobPath::Jump && checkNotBeen(Vector2i(0, -1)) && checkHitboxOk(pos + Vector2i(0, -1), hitboxSize)) {
				beenPoints.insert(pos + Vector2i(0, -1));
				now.second.push_back(MobPath::Jump);
				Q.push(make_pair(pos + Vector2i(0, -1), now.second));
				now.second.pop_back();
			}
		}
		else { // Not solid - fall
			if (checkNotBeen(Vector2i(0, 1))) {
				beenPoints.insert(pos + Vector2i(0, 1));
				now.second.push_back(MobPath::Fall);
				Q.push(make_pair(pos + Vector2i(0, 1), now.second));
				now.second.pop_back();
			}
		}
		if (canClimbLadders&&checkBlockId(pos + Vector2i(0, 1), "ladder")) {
			// If standing on ladder - explictly check falling
			if (checkNotBeen(Vector2i(0, 1))) {
				beenPoints.insert(pos + Vector2i(0, 1));
				now.second.push_back(MobPath::Fall);
				Q.push(make_pair(pos + Vector2i(0, 1), now.second));
				now.second.pop_back();
			}
		}
	}

	return MobPath{ beginPos, targetPos };

}


////////////////////////////////////////
bool MobPathFinder::checkBlockId(Vector2i pos, string id) {
	shared_ptr<Block> b = terrainManager.getBlock(pos);
	return b != nullptr&&b->getBlockId() == id;
}


////////////////////////////////////////
bool MobPathFinder::checkHitboxOk(Vector2i pos, Vector2d hitboxSize) {
	// Borrowed code from Entity::_getBoundingCollisionPoints()
	DoubleRect bound(Vector2d(pos.x + .5 - hitboxSize.x / 2.0, pos.y + 1 - eps - hitboxSize.y), hitboxSize);
	vector<Vector2d> points;

	points.push_back(Vector2d(bound.left + bound.width, bound.top));                  // Right-Top    Point (X-Positive, Y-Negative)
	points.push_back(Vector2d(bound.left + bound.width, bound.top + bound.height));   // Right-Bottom Point (X-Positive, Y-Positive)
	points.push_back(Vector2d(bound.left, bound.top));                                // Left-Top     Point (X-Negative, Y-Negative)
	points.push_back(Vector2d(bound.left, bound.top + bound.height));                 // Left-Bottom  Point (X-Negative, Y-Positive)

	// Add other mesh points to collision points
	// Left   Edge (X-Negative)
	for (double x = 0.5; x < bound.height; x += 0.5)
		points.push_back(Vector2d(bound.left, bound.top + x));
	// Right  Edge (X-Positive)
	for (double x = 0.5; x < bound.height; x += 0.5)
		points.push_back(Vector2d(bound.left + bound.width, bound.top + x));
	// Top    Edge (Y-Negative)
	for (double x = 0.5; x < bound.width; x += 0.5)
		points.push_back(Vector2d(bound.left + x, bound.top));
	// Bottom Edge (Y-Positive)
	for (double x = 0.5; x < bound.width; x += 0.5)
		points.push_back(Vector2d(bound.left + x, bound.top + bound.height));

	// Check point by point
	for (auto& i : points) {
		Vector2i bid(i);
		shared_ptr<Block> b = terrainManager.getBlock(bid);
		if (b != nullptr && b->isSolid()) {
			DoubleRect blockHitbox = b->getHitbox();
			if (blockHitbox.contains(i))
				return false;
		}
	}
	return true;
}

