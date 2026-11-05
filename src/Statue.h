#pragma once
#include "Enemy.h"

// Estatua que crea el jefe final (se dibuja con el sprite de la gárgola):
//   1. Aparece pegada al techo y parpadea WARN_MS para avisar de dónde caerá.
//   2. Cae con gravedad hasta el suelo o una plataforma.
//   3. Se queda quieta STAND_MS y se rompe.
// Solo hace daño mientras cae: avisando en el techo y ya en el suelo es inofensiva.
// Las explosiones la rompen antes.
class Statue : public Enemy {
public:
    static constexpr float WIDTH = 28.f, HEIGHT = 28.f;  // hitbox; el dibujo mide 32x32
    static const int WARN_MS = 700;
    static const int STAND_MS = 4000;

    // position: esquina superior izquierda de la hitbox.
    explicit Statue(const glm::vec2& position);

    // La textura pertenece a la Scene y se comparte entre estatuas.
    void init(Texture* spritesheet, ShaderProgram& program);
    void update(int deltaTime, const movement::Body& player, bool playerFacingLeft) override;
    void render() override;
    bool touches(const movement::Body& other) const override;  // solo mientras cae

protected:
    int deathAnimation() const override;
    glm::vec2 spriteOffset() const override;

private:
    int ageMs = 0;      // tiempo desde que apareció
    int landedMs = -1;  // tiempo en el suelo; -1 mientras no ha aterrizado
};
