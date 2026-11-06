#include <algorithm>
#include <stdexcept>
#include <GL/glew.h>
#include "Hud.h"
#include "Game.h"

namespace {
    const std::string NUMBERS = "0123456789:";
    const std::string STATUS_CHARS = " 0123456789:ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    enum { CLOCK=11, FACE=12, ENEMIES=13, BOMB=14, RANGE=15, GOLD_HEART=16, PANEL=17, CELLS=18 };
    const float SCALE = SCREEN_WIDTH / 160.f;
    const float CELL = 8.f * SCALE;
    const float STATUS_MARGIN = 8.f;
    const float STATUS_ADVANCE = 12.f; // Celda de 6x8 de la fuente, a escala 2.
    const float STATUS_HEIGHT = 16.f;
}

Hud::~Hud() { delete panel; delete glyph; delete statusGlyph; }

void Hud::init(ShaderProgram& program) {
    if (!texture.loadFromFile("images/hud-original-atlas.png",TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/hud-original-atlas.png");
    texture.setMinFilter(GL_NEAREST);
    texture.setMagFilter(GL_NEAREST);
    const glm::vec2 cell(1.f/CELLS,1.f);
    glyph = Sprite::createSprite(glm::vec2(CELL,CELL),cell,&texture,&program);
    glyph->setNumberAnimations(CELLS);
    for (int i=0;i<CELLS;++i) {
        glyph->setAnimationSpeed(i,1);
        glyph->addKeyframe(i,glm::vec2(i*cell.x,0.f));
    }
    panel = Sprite::createSprite(glm::vec2(SCREEN_WIDTH,HEIGHT),cell,&texture,&program);
    panel->setNumberAnimations(1);
    panel->setAnimationSpeed(0,1);
    panel->addKeyframe(0,glm::vec2(PANEL*cell.x,0.f));
    panel->changeAnimation(0);
    panel->setPosition(glm::vec2(0.f,float(SCREEN_HEIGHT-HEIGHT)));

    if (!statusFont.loadFromFile("images/hud-status-bold.png", TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/hud-status-bold.png");
    statusFont.setMinFilter(GL_NEAREST);
    statusFont.setMagFilter(GL_NEAREST);
    const int glyphCount = int(STATUS_CHARS.size());
    const glm::vec2 statusCell(1.f / (glyphCount + 1), 1.f);
    statusGlyph = Sprite::createSprite(glm::vec2(STATUS_ADVANCE, STATUS_HEIGHT), statusCell, &statusFont, &program);
    statusGlyph->setNumberAnimations(glyphCount);
    for (int i = 0; i < glyphCount; ++i) {
        statusGlyph->setAnimationSpeed(i, 1);
        statusGlyph->addKeyframe(i, glm::vec2(i * statusCell.x, 0.f));
    }
}

void Hud::draw(int atlasCell, int nativeX) {
    glyph->changeAnimation(atlasCell);
    glyph->setPosition(glm::vec2(nativeX*SCALE,float(SCREEN_HEIGHT-HEIGHT)));
    glyph->render();
}

void Hud::drawText(const std::string& text, int nativeX) {
    for (char c : text) {
        const auto index = NUMBERS.find(c);
        if (index != std::string::npos) draw(int(index),nativeX);
        nativeX += 8;
    }
}
void Hud::drawNumber(int value, int nativeX) {
    drawText(std::to_string(std::max(0,std::min(99,value))),nativeX);
}

void Hud::drawStatus(const std::string& text, float x) {
    for (char c : text) {
        const auto index = STATUS_CHARS.find(c);
        if (index != std::string::npos) {
            statusGlyph->changeAnimation(int(index));
            statusGlyph->setPosition(glm::vec2(x, STATUS_MARGIN));
            statusGlyph->render();
        }
        x += STATUS_ADVANCE;
    }
}

void Hud::render(const HudInfo& info, ShaderProgram& program) {
    panel->render();
    const int seconds = std::max(0,info.secondsLeft);
    const std::string ss = std::to_string(seconds%60);
    draw(CLOCK,8);
    drawText(std::to_string(seconds/60)+":"+(ss.size()<2?"0":"")+ss,16);
    draw(FACE,64);
    drawNumber(info.spareLives,72);
    if (info.goldHeart) draw(GOLD_HEART,56);
    draw(ENEMIES,88);
    drawNumber(info.enemiesLeft,96);
    draw(BOMB,112);
    drawNumber(info.bombCapacity,120);
    draw(RANGE,136);
    drawNumber(info.fireRange,144);

    if (info.godMode || info.starSeconds > 0) {
        program.setUniform4f("color", 0.f, 0.f, 0.f, 1.f);
        if (info.godMode) drawStatus("GOD MODE", STATUS_MARGIN);
        if (info.starSeconds > 0) {
            const std::string text = "INVENCIBILIDAD: " + std::to_string(info.starSeconds);
            drawStatus(text, SCREEN_WIDTH - STATUS_MARGIN - float(text.size()) * STATUS_ADVANCE);
        }
        program.setUniform4f("color", 1.f, 1.f, 1.f, 1.f);
    }
}
