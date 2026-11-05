#pragma once
#include "Enemy.h"

// Enemigo "Walker" (Koopa Troopa): patrulla su plataforma y gira en paredes y bordes.
// Hace daño por contacto (lo comprueba la Scene).
class Walker : public Enemy {
public:
    static constexpr float WIDTH = 32.f, HEIGHT = 32.f;
    static constexpr float WALK_SPEED = 50.f;  // unidades/s

    // position: esquina superior izquierda del cuerpo de 32x32.
    Walker(const glm::vec2& position, bool facingLeft);

    // La textura pertenece a la Scene y se comparte entre walkers.
    void init(Texture* spritesheet, ShaderProgram& program);
    void update(int deltaTime, const movement::Body& player, bool playerFacingLeft) override;

protected:
    int deathAnimation() const override;
};
