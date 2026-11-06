#include <stdexcept>
#include <GL/glew.h>
#include "LevelSelect.h"
#include "Audio.h"
#include "Game.h"

// level-select.png mide 160x152 y se dibuja a escala 4, como la partida.
// level-buttons.png: botones de 56x32, fila 0 normal y
// fila 1 elegido; la columna es el nivel - 1.
static const float SCALE = 4.f;
static const int BUTTON_W = 56, BUTTON_H = 32;
// Esquina de cada botón en la pantalla, de abajo arriba de la torre.
static const int BUTTON_POS[LevelSelect::LEVEL_COUNT][2] = {
    { 104, 96 },  // 1 Forest
    { 0, 80 },    // 2 Ocean
    { 104, 56 },  // 3 Wind
    { 0, 32 },    // 4 Cloud
    { 104, 16 },  // 5 Evil
};
enum ButtonAnims { NORMAL = 0, SELECTED = LevelSelect::LEVEL_COUNT };

LevelSelect::~LevelSelect() {
    delete button;
}

void LevelSelect::init(ShaderProgram& shaderProgram) {
    program = &shaderProgram;
    background.init("images/level-select.png", shaderProgram);

    if (!buttonsTexture.loadFromFile("images/level-buttons.png", TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/level-buttons.png");
    buttonsTexture.setMinFilter(GL_NEAREST);
    buttonsTexture.setMagFilter(GL_NEAREST);
    const glm::vec2 cell(1.f / LEVEL_COUNT, 1.f / 2.f);
    button = Sprite::createSprite(glm::vec2(BUTTON_W, BUTTON_H) * SCALE, cell, &buttonsTexture, program);
    // Una animación por botón: 0..4 normales y 5..9 elegidos.
    button->setNumberAnimations(2 * LEVEL_COUNT);
    for (int i = 0; i < LEVEL_COUNT; ++i) {
        button->setAnimationSpeed(NORMAL + i, 1);
        button->addKeyframe(NORMAL + i, glm::vec2(i * cell.x, 0.f));
        button->setAnimationSpeed(SELECTED + i, 1);
        button->addKeyframe(SELECTED + i, glm::vec2(i * cell.x, cell.y));
    }
}

int LevelSelect::update() {
    Game& game = Game::instance();
    // Las dos teclas se consumen siempre para que no queden pulsaciones pendientes.
    const bool up = game.consumeKey(GLFW_KEY_UP) | game.consumeKey(GLFW_KEY_W);
    const bool down = game.consumeKey(GLFW_KEY_DOWN) | game.consumeKey(GLFW_KEY_S);
    const int before = selected;
    if (up && selected < LEVEL_COUNT) ++selected;
    if (down && selected > 1) --selected;
    if (selected != before) Audio::instance().play(Audio::MENU_MOVE);
    const bool choose = game.consumeKey(GLFW_KEY_SPACE) | game.consumeKey(GLFW_KEY_ENTER);
    if (!choose || !available[selected - 1]) return NO_CHOICE;  // los bloqueados no suenan
    Audio::instance().play(Audio::MENU_SELECT);
    return selected;
}

void LevelSelect::render() {
    background.render();
    for (int i = 0; i < LEVEL_COUNT; ++i) {
        const bool isSelected = (i + 1 == selected);
        // El fondo ya tiene los botones normales: solo se redibujan el elegido
        // y los bloqueados (oscurecidos con el color del shader).
        if (!isSelected && available[i]) continue;
        const float shade = available[i] ? 1.f : 0.4f;
        program->setUniform4f("color", shade, shade, shade, 1.f);
        button->changeAnimation((isSelected ? SELECTED : NORMAL) + i);
        button->setPosition(glm::vec2(float(BUTTON_POS[i][0]), float(BUTTON_POS[i][1])) * SCALE);
        button->render();
    }
    program->setUniform4f("color", 1.f, 1.f, 1.f, 1.f);
}
