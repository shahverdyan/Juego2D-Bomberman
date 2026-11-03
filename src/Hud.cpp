#include <stdexcept>
#include <GL/glew.h>
#include "Hud.h"
#include "Game.h"

// Mismo orden que CHARS en tools/build_hud_font.py. Tras el último carácter
// hay una celda sólida que se usa como fondo de la franja.
static const std::string FONT_CHARS = " 0123456789:ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const float GLYPH_W = 12.f, GLYPH_H = 16.f;  // celda de 6x8 dibujada al doble
static const float HEART_SIZE = 16.f;

enum HeartAnims { HEART_FULL, HEART_EMPTY };

Hud::~Hud() {
    delete panel;
    delete font;
    delete heart;
}

static void loadPixelTexture(Texture& texture, const char* file) {
    if (!texture.loadFromFile(file, TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error(std::string("Cannot load ") + file);
    texture.setMinFilter(GL_NEAREST);
    texture.setMagFilter(GL_NEAREST);
}

void Hud::init(ShaderProgram& program) {
    loadPixelTexture(fontTexture, "images/hud-font.png");
    loadPixelTexture(heartTexture, "images/Pixel Heart Sprite Sheet 16x16.png");

    const int cells = int(FONT_CHARS.size()) + 1;
    const glm::vec2 cell(1.f / cells, 1.f);

    font = Sprite::createSprite(glm::vec2(GLYPH_W, GLYPH_H), cell, &fontTexture, &program);
    font->setNumberAnimations(int(FONT_CHARS.size()));
    for (int i = 0; i < int(FONT_CHARS.size()); ++i) {
        font->setAnimationSpeed(i, 1);
        font->addKeyframe(i, glm::vec2(i * cell.x, 0.f));
    }

    panel = Sprite::createSprite(glm::vec2(SCREEN_WIDTH, HEIGHT), cell, &fontTexture, &program);
    panel->setNumberAnimations(1);
    panel->setAnimationSpeed(0, 1);
    panel->addKeyframe(0, glm::vec2((cells - 1) * cell.x, 0.f));
    panel->changeAnimation(0);
    panel->setPosition(glm::vec2(0.f, 0.f));

    // Hoja de corazones 48x16: lleno (x=0), medio (x=16, no se usa), vacío (x=32).
    heart = Sprite::createSprite(glm::vec2(HEART_SIZE, HEART_SIZE), glm::vec2(1.f / 3.f, 1.f),
        &heartTexture, &program);
    heart->setNumberAnimations(2);
    heart->setAnimationSpeed(HEART_FULL, 1);
    heart->addKeyframe(HEART_FULL, glm::vec2(0.f, 0.f));
    heart->setAnimationSpeed(HEART_EMPTY, 1);
    heart->addKeyframe(HEART_EMPTY, glm::vec2(2.f / 3.f, 0.f));
}

// Dibuja el texto carácter a carácter reutilizando el mismo sprite.
void Hud::drawText(const std::string& text, float x, float y) {
    for (char c : text) {
        std::size_t index = FONT_CHARS.find(c);
        if (index == std::string::npos) index = 0;  // desconocido: espacio
        font->changeAnimation(int(index));
        font->setPosition(glm::vec2(x, y));
        font->render();
        x += GLYPH_W;
    }
}

void Hud::render(const HudInfo& info) {
    panel->render();

    // Tres columnas, dos filas.
    const float col1 = 8.f, col2 = 220.f, col3 = 420.f;
    const float row1 = 6.f, row2 = 26.f;

    for (int i = 0; i < info.maxLives; ++i) {
        heart->changeAnimation(i < info.lives ? HEART_FULL : HEART_EMPTY);
        heart->setPosition(glm::vec2(col1 + i * (HEART_SIZE + 4.f), row1));
        heart->render();
    }

    const int seconds = info.secondsLeft < 0 ? 0 : info.secondsLeft;
    const std::string ss = std::to_string(seconds % 60);
    drawText("TIEMPO " + std::to_string(seconds / 60) + ":" + (ss.size() < 2 ? "0" : "") + ss,
        col2, row1);
    drawText("NIVEL " + std::to_string(info.level), col3, row1);
    if (info.invulnerable) drawText("INMUNE", 556.f, row1);

    drawText("ENEMIGOS " + std::to_string(info.enemiesLeft), col1, row2);
    drawText("EXPLOSION " + std::to_string(info.fireRange), col2, row2);
    drawText("BOMBAS " + std::to_string(info.bombCapacity), col3, row2);
}
