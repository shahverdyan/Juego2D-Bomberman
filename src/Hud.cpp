#include <algorithm>
#include <stdexcept>
#include <GL/glew.h>
#include "Hud.h"
#include "Game.h"

namespace {
    const std::string NUMBERS = "0123456789:";
    enum { CLOCK=11, FACE=12, ENEMIES=13, BOMB=14, RANGE=15, GOLD_HEART=16, PANEL=17, CELLS=18 };
    const float SCALE = SCREEN_WIDTH / 160.f;
    const float CELL = 8.f * SCALE;
}

Hud::~Hud() { delete panel; delete glyph; }

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

void Hud::render(const HudInfo& info) {
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
}
