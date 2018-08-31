#pragma once

#include "Main.hpp"

struct MobPath {
	enum Action {
		MoveRight,
		MoveLeft,
		Jump,
		Fall,
		AscendLadder,
		DescendLadder,
		Count
	};

	Vector2i beginPos, endPos;
	vector<Action> actions;
};


class MobPathFinder {
public:
	static MobPath findPath(Vector2i beginPos, Vector2i targetPos, Vector2d hitboxSize, bool canClimbLadders = false, int maxSteps = 100);
private:
	static bool checkBlockId(Vector2i pos, string id);
	static bool checkHitboxOk(Vector2i pos, Vector2d hitboxSize);
};


