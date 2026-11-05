#pragma once
#include "Enemy.h"

// Enemigo "Boo" (fantasma de Mario): vuela sin gravedad y atraviesa el terreno.
// Si el jugador le da la espalda, acelera hacia él; si el jugador lo mira,
// frena hasta pararse y se tapa la cara. Siempre se gira hacia el jugador.
// Solo despierta cuando el jugador está cerca, para no cruzar todo el mapa.
// Hace daño por contacto y muere con las explosiones (lo comprueba la Scene).
class Boo : public Enemy {
public:
    static constexpr float WIDTH = 28.f, HEIGHT = 24.f;  // hitbox algo menor que el dibujo
    static constexpr float ACCELERATION = 150.f;  // unidades/s² al perseguir
    static constexpr float BRAKE = 300.f;         // unidades/s² al frenar
    static constexpr float MAX_SPEED = 70.f;      // unidades/s (el jugador anda a 160)
    static constexpr float WAKE_DISTANCE = 320.f; // 10 tiles

    // position: esquina superior izquierda de la hitbox.
    Boo(const glm::vec2& position, bool facingLeft);

    // La textura pertenece a la Scene y se comparte entre boos.
    void init(Texture* spritesheet, ShaderProgram& program);
    void update(int deltaTime, const movement::Body& player, bool playerFacingLeft) override;
    void render() override;

protected:
    int deathAnimation() const override;
    // Al morir no cae: sube despacio mientras parpadea, como si se desvaneciera.
    void moveWhileDying(float seconds) override;
    glm::vec2 spriteOffset() const override;

private:
    float speedX = 0.f, speedY = 0.f;
};
