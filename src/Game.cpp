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
}

bool Game::update(int deltaTime)
{
	if (consumeKey(GLFW_KEY_R)) scene.resetPlayer();
	if (consumeKey(GLFW_KEY_G)) scene.toggleInvulnerability();
	scene.update(deltaTime);

	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.render();
}

void Game::keyPressed(int key)
{
	if(key == GLFW_KEY_ESCAPE) // Escape code
		bPlay = false;
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
void Game::shutdown() { scene.shutdown(); }

void Game::startMapEffectsTest() { scene.startMapEffectsTest(); }
