#pragma once
#include "Screen.h"

// Selección de nivel con la pantalla original de selección de mundo: una torre
// con cinco botones (Forest, Ocean, Wind, Cloud y Evil = niveles 1 a 5).
// ↑/↓ (o W/S) suben y bajan por la torre y Espacio/Enter empiezan el nivel.
// Los niveles bloqueados (sin superar el anterior, o que aún no existen) se ven
// oscurecidos y no se pueden elegir.
class LevelSelect {
public:
    static const int LEVEL_COUNT = 5;
    static const int NO_CHOICE = 0;

    LevelSelect() = default;
    ~LevelSelect();
    LevelSelect(const LevelSelect&) = delete;
    LevelSelect& operator=(const LevelSelect&) = delete;

    void init(ShaderProgram& program);
    void setAvailable(int level, bool available) { this->available[level - 1] = available; }
    // Devuelve el número de nivel elegido en este frame (1..5), o NO_CHOICE.
    int update();
    void render();

private:
    Screen background;
    Texture buttonsTexture;
    Sprite* button = nullptr;
    ShaderProgram* program = nullptr;
    bool available[LEVEL_COUNT] = {};
    int selected = 1;
};
