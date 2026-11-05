#pragma once
#include "Sprite.h"
#include "Movement.h"

// Puerta-portal del final del nivel. Empieza cerrada; la Scene la abre cuando
// no quedan enemigos. Reproduce la animación de apertura y después el portal
// queda girando. Si el jugador la toca abierta, completa el nivel.
class Door {
public:
    enum State { CLOSED, OPENING, OPEN };
    static constexpr float WIDTH = 56.f, HEIGHT = 64.f;  // 28x32 píxeles a escala 2
    static const int OPENING_FPS = 6;
    static const int OPENING_MS = 4 * 1000 / OPENING_FPS;  // 4 fotogramas de apertura

    Door() = default;
    ~Door();
    Door(const Door&) = delete;
    Door& operator=(const Door&) = delete;

    // bottomCenter: punto medio del borde inferior (sobre el suelo).
    void init(Texture* spritesheet, ShaderProgram& program, const glm::vec2& bottomCenter);
    void open();  // empieza a abrirse si está cerrada
    void update(int deltaTime);
    void render();

    bool isOpen() const { return state == OPEN; }
    // ¿El centro del jugador está dentro de la puerta?
    bool contains(const movement::Body& body) const;

private:
    State state = CLOSED;
    int openingMs = 0;
    glm::vec2 position;  // esquina superior izquierda
    Sprite* sprite = nullptr;
};
