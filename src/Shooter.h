#pragma once
#include <vector>
#include "Enemy.h"

// Enemigo "Shooter": patrulla a izquierda/derecha sobre su plataforma y,
// cuando ve al jugador delante en su misma fila, se para y le dispara balas
// horizontales. La Scene decide qué pasa cuando una bala toca al jugador.
class Shooter : public Enemy {
public:
    struct Bullet { float x, y, vx; };

    static constexpr float WIDTH = 32.f, HEIGHT = 48.f;
    static constexpr float WALK_SPEED = 60.f;        // unidades/s
    static constexpr float VISION_RANGE = 5 * 32.f;  // 5 tiles (media pantalla)
    static constexpr float BULLET_SPEED = 160.f;     // unidades/s
    static constexpr float BULLET_SIZE = 16.f;
    static constexpr float MUZZLE_Y = 26.f;          // altura de la boca respecto a y
    static constexpr int REACTION_MS = 400;          // espera antes del primer disparo
    static constexpr int COOLDOWN_MS = 1200;         // tiempo entre disparos
    static constexpr int FIRE_ANIM_MS = 250;

    // position: esquina superior izquierda del cuerpo de 32x48.
    Shooter(const glm::vec2& position, bool facingLeft);
    ~Shooter() override;

    // Las texturas pertenecen a la Scene y se comparten entre shooters.
    void init(Texture* spritesheet, Texture* bulletTexture, ShaderProgram& program);
    void update(int deltaTime, const movement::Body& player, bool playerFacingLeft) override;
    int deathAnimation() const override;
    void render() override;

    // Devuelve true si alguna bala toca al jugador; esa bala desaparece.
    bool hitsPlayer(const movement::Body& player) override;
    void clearProjectiles() override { bullets.clear(); }

private:
    bool canSee(const movement::Body& player) const;
    void fire();
    void updateBullets(float seconds);

    bool seesPlayer = false;
    int shootTimerMs = REACTION_MS;
    int fireAnimMs = 0;
    std::vector<Bullet> bullets;
    Sprite* bulletSprite = nullptr;
};
