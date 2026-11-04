#pragma once
#include <string>
#include "Sprite.h"
#include "ViewConfig.h"

// Datos que muestra el HUD. La Scene los rellena en cada fotograma.
struct HudInfo {
    int lives, maxLives;
    int secondsLeft;
    int enemiesLeft;
    int level;
    int fireRange;
    int bombCapacity;
    bool invulnerable;  // modo G
    int starSeconds;    // segundos de estrella restantes (0 = sin estrella)
};

// Franja superior de la pantalla con el estado de la partida.
// Se dibuja en coordenadas de pantalla (la Scene cambia la proyección antes).
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
    void drawText(const std::string& text, float x, float y);

    Texture fontTexture, heartTexture;
    Sprite* panel = nullptr;   // fondo de la franja
    Sprite* font = nullptr;    // una animación por carácter
    Sprite* heart = nullptr;   // animación 0 lleno, 1 vacío
};
