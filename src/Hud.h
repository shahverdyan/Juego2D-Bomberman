#pragma once
#include <string>
#include "Sprite.h"
#include "ViewConfig.h"

struct HudInfo {
    int spareLives;
    bool goldHeart;
    int secondsLeft;
    int enemiesLeft;
    int fireRange;
    int bombCapacity;
};

// Original-style 160x8 strip, displayed at integer scale along the bottom.
class Hud {
public:
    static const int HEIGHT = view::HUD_HEIGHT;
    Hud() {}
    ~Hud();
    Hud(const Hud&) = delete;
    Hud& operator=(const Hud&) = delete;
    void init(ShaderProgram& program);
    void render(const HudInfo& info);
private:
    void draw(int atlasCell, int nativeX);
    void drawText(const std::string& text, int nativeX);
    void drawNumber(int value, int nativeX);
    Texture texture;
    Sprite* panel = nullptr;
    Sprite* glyph = nullptr;
};
