#pragma once

#include "Main.hpp"

// Handled by game scene (GameScene or TestScene)
struct RenderIO {

	// Size of a block, in pixels
	double gameScaleFactor;

	// Logical on-screen position of point (0, 0)
	// Handled by game scene
	Vector2d gameScenePosOffset;

	// Rectangle covering current rendering area
	// Handled by game scene
	FloatRect viewRect;
	DoubleRect viewRectBlock;

};

// Handled by scene manager ( App class )
struct LogicIO {

	enum KeyState {
		Released,
		JustPressed,
		Pressed,
		JustReleased
	};

	// Elapsed time before last logic update
	Time deltaTime;

	// Error message returned by last failed operation
	string lastError;

	// Size of the render window area
	Vector2i renderSize;

	// Does the window have focus?
	bool hasFocus;


	// State of all the keys (pressed or released) in the last frame
	// Set by SFML coding (Keyboard::Key or Mouse::Button)
	// HACK Handled by game scene for pausing game issues
	KeyState keyboardState[Keyboard::KeyCount];
	KeyState mouseState[Mouse::ButtonCount];

	// On-screen position of the mouse cursor
	// HACK Also handled by game scene for pausing game
	Vector2i mousePos;
};

// Handled by game scene (GameScene or TestScene)
struct GameIO {

	// Offset angle from the X-axis to the line between the player and the cursor
	// in clockwise pattern
	double degreeAngle;

	// Game rules
	// Does explosions damage terrain?
	bool ruleExplosionDamagesTerrain;

};

RenderIO renderIO;
LogicIO logicIO;
GameIO gameIO;

