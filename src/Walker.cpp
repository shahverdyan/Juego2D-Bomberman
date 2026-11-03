#include "Walker.h"

// Sprite provisional: bub.png (4x4 celdas de 0.25). Columna 0 mira a la
// izquierda y columna 1 a la derecha; filas 0-2 son los pasos.
enum WalkerAnims { WALK_LEFT, WALK_RIGHT };

Walker::Walker(const glm::vec2& position, bool facingLeft)
    : Enemy(position, WIDTH, HEIGHT, facingLeft) {}

void Walker::init(Texture* spritesheet, ShaderProgram& program) {
    sprite = Sprite::createSprite(glm::vec2(WIDTH, HEIGHT), glm::vec2(0.25f, 0.25f),
        spritesheet, &program);
    sprite->setNumberAnimations(2);
    for (int side = 0; side < 2; ++side) {
        sprite->setAnimationSpeed(WALK_LEFT + side, 6);
        for (int step = 0; step < 3; ++step)
            sprite->addKeyframe(WALK_LEFT + side, glm::vec2(side * 0.25f, step * 0.25f));
    }
    sprite->changeAnimation(facingLeft ? WALK_LEFT : WALK_RIGHT);
    sprite->setPosition(glm::vec2(body.x, body.y));
}

// El jugador no influye en su comportamiento.
void Walker::update(int deltaTime, const movement::Body&) {
    const float seconds = deltaTime / 1000.f;
    if (walk(WALK_SPEED, seconds)) facingLeft = !facingLeft;
    applyGravity(seconds);
    updateSprite(deltaTime, facingLeft ? WALK_LEFT : WALK_RIGHT);
}
