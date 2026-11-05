#include <stdexcept>
#include <GL/glew.h>
#include "Menu.h"
#include "Audio.h"
#include "Game.h"

// Posición del texto de la primera opción y separación entre opciones, en píxeles
// de una pantalla de 320x304 (la ventana a escala 2). Si se cambia la imagen
// menu-background.jpeg, hay que ajustar estos valores para que el cursor cuadre.
static const float SCALE = 2.f;
static const int OPTION_X = 196, OPTION_Y = 214, OPTION_SPACING = 18;
static const int CURSOR_SIZE = 8;

Menu::~Menu() {
    delete cursor;
}

void Menu::init(ShaderProgram& program) {
    background.init("images/menu-background.jpeg", program);

    if (!cursorTexture.loadFromFile("images/menu-cursor.png", TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/menu-cursor.png");
    cursorTexture.setMinFilter(GL_NEAREST);
    cursorTexture.setMagFilter(GL_NEAREST);
    cursor = Sprite::createSprite(glm::vec2(CURSOR_SIZE * SCALE), glm::vec2(1.f, 1.f),
        &cursorTexture, &program);
    cursor->setNumberAnimations(1);
    cursor->setAnimationSpeed(0, 1);
    cursor->addKeyframe(0, glm::vec2(0.f, 0.f));
    cursor->changeAnimation(0);
    selected = PLAY;
}

int Menu::update(int deltaTime) {
    timeMs += deltaTime;
    Game& game = Game::instance();
    // Las dos teclas se consumen siempre para que no queden pulsaciones pendientes.
    const bool up = game.consumeKey(GLFW_KEY_UP) | game.consumeKey(GLFW_KEY_W);
    const bool down = game.consumeKey(GLFW_KEY_DOWN) | game.consumeKey(GLFW_KEY_S);
    if (up) selected = (selected + OPTION_COUNT - 1) % OPTION_COUNT;
    if (down) selected = (selected + 1) % OPTION_COUNT;
    if (up || down) Audio::instance().play(Audio::MENU_MOVE);
    const bool choose = game.consumeKey(GLFW_KEY_SPACE) | game.consumeKey(GLFW_KEY_ENTER);
    if (choose) Audio::instance().play(Audio::MENU_SELECT);
    return choose ? selected : NO_CHOICE;
}

void Menu::render() {
    background.render();
    // La bomba se mueve 1 píxel a izquierda y derecha para llamar la atención.
    const int bob = (timeMs / 250) % 2;
    const float x = float(OPTION_X - CURSOR_SIZE - 4 - bob);
    const float y = float(OPTION_Y - 1 + selected * OPTION_SPACING);
    cursor->setPosition(glm::vec2(x, y) * SCALE);
    cursor->render();
}
