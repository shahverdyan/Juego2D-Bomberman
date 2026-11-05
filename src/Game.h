#ifndef _GAME_INCLUDE
#define _GAME_INCLUDE


#include <GLFW/glfw3.h>
#include "Scene.h"
#include "Audio.h"
#include "Menu.h"
#include "LevelSelect.h"
#include "Screen.h"
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
	// Estados del juego: cada uno decide qué se actualiza y qué se dibuja.
	enum GameState { MENU, INSTRUCTIONS, CREDITS, LEVEL_SELECT, PLAYING, GAME_OVER, VICTORY };
	void changeState(GameState newState);

	// Transición con fundido a negro: la pantalla se oscurece durante FADE_MS, se
	// aplica el cambio (estado y, si hace falta, nivel) y vuelve a aparecer en FADE_MS.
	// Mientras dura no se actualiza nada.
	static const int FADE_MS = 300;
	enum FadePhase { NO_FADE, FADE_OUT, FADE_IN };
	// jingle: pista que suena una vez al llegar al nuevo estado (nivel superado, game over...).
	void fadeTo(GameState newState, int newLevel = 0, bool newGame = false,
		Audio::Music jingle = Audio::NO_MUSIC);
	void applyPendingChange();
	void renderFade();

	// Progreso: solo se puede elegir un nivel tras superar el anterior.
	// unlockedLevel es el nivel más alto desbloqueado (se pierde al cerrar el juego).
	static int nextLevel(int level);  // siguiente nivel que existe, o 0 si no hay más
	void unlockLevel(int level);
	void refreshLevelSelect();
	void updateMusic();
	int unlockedLevel = 1;

	FadePhase fadePhase = NO_FADE;
	int fadeMs = 0;
	GameState pendingState = MENU;
	int pendingLevel = 0;         // nivel a cargar en mitad del fundido (0 = ninguno)
	bool pendingNewGame = false;  // true: partida nueva (vidas y mejoras iniciales)
	Audio::Music pendingJingle = Audio::NO_MUSIC;
	Texture* fadeTexture = nullptr;
	Sprite* fadeSprite = nullptr;  // cuadro negro del tamaño de la ventana

	GameState state = MENU;
	Menu* menu = nullptr;
	Screen* instructions = nullptr;
	Screen* credits = nullptr;
	LevelSelect* levelSelect = nullptr;
	Screen* gameOver = nullptr;
	Screen* victory = nullptr;
	bool bPlay; // Continue to play game?
	bool keys[GLFW_KEY_LAST+1] = {};
    bool pressed[GLFW_KEY_LAST+1] = {}; // Store key states so that
							    // we can have access at any time
	Scene scene;

};


#endif // _GAME_INCLUDE


