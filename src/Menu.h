#pragma once
#include "Screen.h"

// Menú principal: fondo fijo (menu-background.jpeg) y una bomba que hace de
// cursor. ↑/↓ (o W/S) cambian la opción y Espacio/Enter la eligen.
class Menu {
public:
    enum Option { PLAY, INSTRUCTIONS, CREDITS, OPTION_COUNT };
    static const int NO_CHOICE = -1;

    Menu() = default;
    ~Menu();
    Menu(const Menu&) = delete;
    Menu& operator=(const Menu&) = delete;

    void init(ShaderProgram& program);
    // Devuelve la opción elegida en este frame, o NO_CHOICE.
    int update(int deltaTime);
    void render();

private:
    Screen background;
    Texture cursorTexture;
    Sprite* cursor = nullptr;
    int selected = PLAY;
    int timeMs = 0;
};
