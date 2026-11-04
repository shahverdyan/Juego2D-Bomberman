#ifndef _GAME_INCLUDE
#define _GAME_INCLUDE


#include <GLFW/glfw3.h>
#include "Scene.h"
#include "ViewConfig.h"


#define SCREEN_WIDTH view::WINDOW_WIDTH
#define SCREEN_HEIGHT view::WINDOW_HEIGHT


// Game is a singleton (a class with a single instance) that represents our whole application


class Game
{

private:
	Game() {}

public:
	static Game &instance()
	{
		static Game G;

		return G;
	}

	void init();
    void shutdown();
    void startMapEffectsTest();
    void startOriginalEffectsTest();
    void runHudTestStep(int step);
    void runBombCollisionTest();
	bool update(int deltaTime);
	void render();

	// Input callback methods
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMove(int x, int y);
	void mousePress(int button);
	void mouseRelease(int button);

	bool getKey(int key) const;
    bool consumeKey(int key);

private:
	bool bPlay; // Continue to play game?
	bool keys[GLFW_KEY_LAST+1] = {};
    bool pressed[GLFW_KEY_LAST+1] = {}; // Store key states so that
							    // we can have access at any time
	Scene scene;

};


#endif // _GAME_INCLUDE


