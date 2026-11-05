#include <algorithm>
#include <cmath>
#include "Boo.h"

// boo-atlas.png (tools/build_boo.py): 6 columnas x 2 filas de 20x16 píxeles.
// Fila 0 mira a la izquierda y fila 1 a la derecha.
// Columnas 0-3 persiguiendo, columna 4 escondido, columna 5 muerte (ojos cerrados).
enum BooAnims { CHASE_LEFT, CHASE_RIGHT, HIDE_LEFT, HIDE_RIGHT, DEATH_LEFT, DEATH_RIGHT };

// Dibujo de 40x32 (20x16 píxeles a escala 2), centrado sobre la hitbox de 28x24.
static const glm::vec2 SPRITE_SIZE(40.f, 32.f);
static const glm::vec2 SPRITE_OFFSET((Boo::WIDTH - 40.f) / 2.f, (Boo::HEIGHT - 32.f) / 2.f);

Boo::Boo(const glm::vec2& position, bool facingLeft)
    : Enemy(position, WIDTH, HEIGHT, facingLeft) {}

void Boo::init(Texture* spritesheet, ShaderProgram& program) {
    const float cellW = 1.f / 6.f, cellH = 1.f / 2.f;
    sprite = Sprite::createSprite(SPRITE_SIZE, glm::vec2(cellW, cellH), spritesheet, &program);
    sprite->setNumberAnimations(6);
    for (int side = 0; side < 2; ++side) {
        sprite->setAnimationSpeed(CHASE_LEFT + side, 6);
        for (int frame = 0; frame < 4; ++frame)
            sprite->addKeyframe(CHASE_LEFT + side, glm::vec2(frame * cellW, side * cellH));
        sprite->setAnimationSpeed(HIDE_LEFT + side, 1);
        sprite->addKeyframe(HIDE_LEFT + side, glm::vec2(4 * cellW, side * cellH));
        sprite->setAnimationSpeed(DEATH_LEFT + side, 1);
        sprite->addKeyframe(DEATH_LEFT + side, glm::vec2(5 * cellW, side * cellH));
    }
    sprite->changeAnimation(facingLeft ? HIDE_LEFT : HIDE_RIGHT);
    sprite->setPosition(glm::vec2(body.x, body.y) + spriteOffset());
}

void Boo::update(int deltaTime, const movement::Body& player, bool playerFacingLeft) {
    const float seconds = deltaTime / 1000.f;

    // Vector desde el centro del Boo hasta el centro del jugador.
    const float dx = (player.x + player.width / 2.f) - (body.x + body.width / 2.f);
    const float dy = (player.y + player.height / 2.f) - (body.y + body.height / 2.f);
    const float distance = std::sqrt(dx * dx + dy * dy);

    // Siempre mira hacia el jugador.
    if (dx != 0.f) facingLeft = dx < 0.f;

    // El jugador lo mira si está a su izquierda y mira a la derecha, o al revés.
    const bool playerLooksAtBoo = (dx < 0.f) != playerFacingLeft;
    const bool chasing = !playerLooksAtBoo && distance > 1.f && distance <= WAKE_DISTANCE;

    if (chasing) {
        // Acelera en la dirección del jugador sin pasar de la velocidad máxima.
        speedX += dx / distance * ACCELERATION * seconds;
        speedY += dy / distance * ACCELERATION * seconds;
        const float speed = std::sqrt(speedX * speedX + speedY * speedY);
        if (speed > MAX_SPEED) {
            speedX *= MAX_SPEED / speed;
            speedY *= MAX_SPEED / speed;
        }
    } else {
        // Frena poco a poco hasta quedarse quieto.
        const float speed = std::sqrt(speedX * speedX + speedY * speedY);
        const float newSpeed = std::max(0.f, speed - BRAKE * seconds);
        if (speed > 0.f) {
            speedX *= newSpeed / speed;
            speedY *= newSpeed / speed;
        }
    }

    // Vuela a través del terreno: no hay colisión con el mapa.
    body.x += speedX * seconds;
    body.y += speedY * seconds;

    const int side = facingLeft ? 0 : 1;
    updateSprite(deltaTime, (chasing ? CHASE_LEFT : HIDE_LEFT) + side);
}

void Boo::render() {
    // Al morir parpadea, cada vez más rápido, hasta desaparecer.
    if (dying) {
        const int blinkMs = deathMs < DEATH_MS / 2 ? 120 : 60;
        if ((deathMs / blinkMs) % 2 == 1) return;
    }
    Enemy::render();
}

int Boo::deathAnimation() const {
    return facingLeft ? DEATH_LEFT : DEATH_RIGHT;
}

void Boo::moveWhileDying(float seconds) {
    body.y -= 30.f * seconds;
}

glm::vec2 Boo::spriteOffset() const {
    return SPRITE_OFFSET;
}
