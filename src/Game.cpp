#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Game.h"


void Game::init()
{
	bPlay = true;
	glClearColor(0.08f, 0.12f, 0.18f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	scene.init();
	// Las pantallas comparten el shader de la Scene.
	menu = new Menu();
	menu->init(scene.getShaderProgram());
	instructions = new Screen();
	instructions->init("images/menu-instructions.png", scene.getShaderProgram());
	credits = new Screen();
	credits->init("images/menu-credits.png", scene.getShaderProgram());
	levelSelect = new LevelSelect();
	levelSelect->init(scene.getShaderProgram());
	refreshLevelSelect();
	gameOver = new Screen();
	gameOver->init("images/game-over.jpeg", scene.getShaderProgram());
	victory = new Screen();
	victory->init("images/victory.jpeg", scene.getShaderProgram());

	// Cuadro negro para los fundidos: una textura de 1x1 basta, porque el color
	// final lo pone el uniform "color" (negro con la transparencia del fundido).
	unsigned char white = 255;
	fadeTexture = new Texture();
	fadeTexture->loadFromGlyphBuffer(&white, 1, 1);
	fadeSprite = Sprite::createSprite(glm::vec2(float(SCREEN_WIDTH), float(SCREEN_HEIGHT)),
		glm::vec2(1.f, 1.f), fadeTexture, &scene.getShaderProgram());
	fadeSprite->setNumberAnimations(1);
	fadeSprite->setAnimationSpeed(0, 1);
	fadeSprite->addKeyframe(0, glm::vec2(0.f, 0.f));
	fadeSprite->changeAnimation(0);
	fadeSprite->setPosition(glm::vec2(0.f, 0.f));

	changeState(MENU);
}

// Cambia de estado y descarta las teclas pulsadas en el anterior, para que
// por ejemplo el Espacio que elige "Jugar" no ponga también una bomba.
void Game::changeState(GameState newState)
{
	state = newState;
	Audio::instance().stopJingle();  // el jingle de la pantalla anterior no sigue sonando
	for (bool& key : pressed) key = false;
}

// Empieza un fundido a negro. El cambio se aplica cuando la pantalla está negra.
void Game::fadeTo(GameState newState, int newLevel, bool newGame, Audio::Music jingle)
{
	pendingState = newState;
	pendingJingle = jingle;
	pendingLevel = newLevel;
	pendingNewGame = newGame;
	fadePhase = FADE_OUT;
	fadeMs = 0;
}

void Game::applyPendingChange()
{
	if (pendingLevel != 0) {
		if (pendingNewGame) scene.startNewGame(pendingLevel);
		else scene.startLevel(pendingLevel);
	}
	changeState(pendingState);
	if (pendingJingle != Audio::NO_MUSIC) Audio::instance().playMusic(pendingJingle, false);
}

// Música de fondo de cada pantalla. Si suena un jingle se espera a que acabe:
// así, al pasar de nivel, primero suena la fanfarria y después la música del nivel.
void Game::updateMusic()
{
	Audio& audio = Audio::instance();
	if (audio.isJinglePlaying()) return;
	Audio::Music music = Audio::NO_MUSIC;  // game over y victoria: solo su jingle
	if (state == MENU || state == INSTRUCTIONS || state == CREDITS || state == LEVEL_SELECT)
		music = Audio::MENU_MUSIC;
	else if (state == PLAYING)
		music = scene.isBossLevel() ? Audio::BOSS_MUSIC : Audio::LEVEL_MUSIC;
	if (music == Audio::NO_MUSIC) audio.stopMusic();
	else audio.playMusic(music, true);  // no hace nada si ya suena
}

// Siguiente nivel con archivo. Mientras falten niveles intermedios se saltan,
// para que el último (el jefe) siga siendo alcanzable.
int Game::nextLevel(int level)
{
	for (int next = level + 1; next <= Scene::LEVEL_COUNT; ++next)
		if (Scene::levelExists(next)) return next;
	return 0;
}

void Game::unlockLevel(int level)
{
	if (level > unlockedLevel) unlockedLevel = level;
	refreshLevelSelect();
}

// Se pueden elegir los niveles que existen y ya están desbloqueados.
void Game::refreshLevelSelect()
{
	for (int level = 1; level <= LevelSelect::LEVEL_COUNT; ++level)
		levelSelect->setAvailable(level, Scene::levelExists(level) && level <= unlockedLevel);
}

bool Game::update(int deltaTime)
{
	updateMusic();

	// Durante un fundido solo avanza la transición.
	if (fadePhase != NO_FADE) {
		fadeMs += deltaTime;
		if (fadeMs >= FADE_MS) {
			fadeMs = 0;
			if (fadePhase == FADE_OUT) {
				applyPendingChange();
				fadePhase = FADE_IN;
			}
			else fadePhase = NO_FADE;
		}
		return bPlay;
	}

	// Teclas 1-5 (atajo de prueba): cargan ese nivel desde cualquier pantalla, aunque
	// esté bloqueado. La sala del jefe (nivel 6) solo se alcanza superando el 5.
	for (int number = 1; number <= LevelSelect::LEVEL_COUNT; ++number) {
		if (consumeKey(GLFW_KEY_0 + number) && Scene::levelExists(number)) {
			unlockLevel(number);
			fadeTo(PLAYING, number, state != PLAYING);  // desde un menú, partida nueva
			return bPlay;
		}
	}

	switch (state) {
	case MENU: {
		if (consumeKey(GLFW_KEY_ESCAPE)) bPlay = false;
		const int choice = menu->update(deltaTime);
		if (choice == Menu::PLAY) fadeTo(LEVEL_SELECT);
		else if (choice == Menu::INSTRUCTIONS) fadeTo(INSTRUCTIONS);
		else if (choice == Menu::CREDITS) fadeTo(CREDITS);
		break;
	}
	case INSTRUCTIONS:
	case CREDITS:
		if (consumeKey(GLFW_KEY_ESCAPE) | consumeKey(GLFW_KEY_SPACE) | consumeKey(GLFW_KEY_ENTER))
			fadeTo(MENU);
		break;
	case LEVEL_SELECT: {
		if (consumeKey(GLFW_KEY_ESCAPE)) {
			fadeTo(MENU);
			break;
		}
		const int level = levelSelect->update();
		if (level != LevelSelect::NO_CHOICE) fadeTo(PLAYING, level, true);
		break;
	}
	case PLAYING:
		// ESC abandona la partida y vuelve a la selección de nivel (el menú de juego).
		if (consumeKey(GLFW_KEY_ESCAPE)) {
			fadeTo(LEVEL_SELECT);
			break;
		}
		if (consumeKey(GLFW_KEY_R)) scene.resetPlayer();
		if (consumeKey(GLFW_KEY_G)) scene.toggleInvulnerability();
		if (consumeKey(GLFW_KEY_K)) scene.killAllEnemies();
		scene.update(deltaTime);
		if (scene.getResult() == Scene::GAME_OVER) fadeTo(GAME_OVER, 0, false, Audio::GAME_OVER_JINGLE);
		else if (scene.getResult() == Scene::LEVEL_COMPLETED) {
			// Desbloquea y pasa al siguiente nivel; si no hay más, se ha ganado la partida.
			const int next = nextLevel(scene.getLevel());
			if (next != 0) {
				unlockLevel(next);
				// Al pasar a la sala del jefe (el último nivel) no suena el jingle:
				// se entra directamente con la música del jefe.
				const bool toBoss = (next == Scene::LEVEL_COUNT);
				fadeTo(PLAYING, next, false, toBoss ? Audio::NO_MUSIC : Audio::LEVEL_CLEAR_JINGLE);
			}
			else fadeTo(VICTORY, 0, false, Audio::VICTORY_JINGLE);
		}
		break;
	case GAME_OVER:
        if (consumeKey(GLFW_KEY_ESCAPE) | consumeKey(GLFW_KEY_SPACE) | consumeKey(GLFW_KEY_ENTER))
            fadeTo(MENU);
        break;
	case VICTORY:
		if (consumeKey(GLFW_KEY_ESCAPE) | consumeKey(GLFW_KEY_SPACE) | consumeKey(GLFW_KEY_ENTER))
			fadeTo(LEVEL_SELECT);
		break;
	}
	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	switch (state) {
	case MENU:         menu->render(); break;
	case INSTRUCTIONS: instructions->render(); break;
	case CREDITS:      credits->render(); break;
	case LEVEL_SELECT: levelSelect->render(); break;
	case PLAYING:      scene.render(); break;
	case GAME_OVER:    gameOver->render(); break;
	case VICTORY:      victory->render(); break;
	}
	if (fadePhase != NO_FADE) renderFade();
}

// Cuadro negro encima de todo: va de transparente a opaco (FADE_OUT) y al revés (FADE_IN).
void Game::renderFade()
{
	const float t = float(fadeMs) / FADE_MS;
	const float alpha = (fadePhase == FADE_OUT) ? t : 1.f - t;
	ShaderProgram& program = scene.getShaderProgram();
	Screen::beginFullWindow(program);
	program.setUniform4f("color", 0.f, 0.f, 0.f, alpha);
	fadeSprite->render();
	program.setUniform4f("color", 1.f, 1.f, 1.f, 1.f);
}

void Game::keyPressed(int key)
{
	if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (!keys[key]) pressed[key] = true;
        keys[key] = true;
    }
}

void Game::keyReleased(int key)
{
	if (key >= 0 && key <= GLFW_KEY_LAST) keys[key] = false;
}

void Game::mouseMove(int x, int y)
{
}

void Game::mousePress(int button)
{
}

void Game::mouseRelease(int button)
{
}

bool Game::getKey(int key) const
{
	return key >= 0 && key <= GLFW_KEY_LAST && keys[key];
}




bool Game::consumeKey(int key) {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    bool value = pressed[key]; pressed[key] = false; return value;
}
// Las pantallas se liberan aquí, mientras el contexto de OpenGL sigue vivo.
void Game::shutdown() {
    delete menu; menu = nullptr;
    delete instructions; instructions = nullptr;
    delete credits; credits = nullptr;
    delete levelSelect; levelSelect = nullptr;
    delete gameOver; gameOver = nullptr;
    delete victory; victory = nullptr;
    delete fadeSprite; fadeSprite = nullptr;
    delete fadeTexture; fadeTexture = nullptr;
    scene.shutdown();
}

// Las pruebas ocultas empiezan directamente en la partida, sin menú.
void Game::startMapEffectsTest() { changeState(PLAYING); scene.startMapEffectsTest(); }

void Game::runHudTestStep(int step) { if (step == 0) changeState(PLAYING); scene.runHudTestStep(step); }

void Game::runBombCollisionTest() { changeState(PLAYING); scene.runBombCollisionTest(); }

void Game::startOriginalEffectsTest() { changeState(PLAYING); scene.startOriginalEffectsTest(); }
