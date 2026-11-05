#include <cmath>
#include "Enemy.h"

Enemy::Enemy(const glm::vec2& position, float width, float height, bool facingLeft)
    : facingLeft(facingLeft) {
    body.x = position.x;
    body.y = position.y;
    body.width = width;
    body.height = height;
}

Enemy::~Enemy() {
    delete sprite;
}

void Enemy::render() {
    sprite->render();
}

bool Enemy::touches(const movement::Body& other) const {
    return body.x < other.x + other.width && body.x + body.width > other.x &&
        body.y < other.y + other.height && body.y + body.height > other.y;
}

void Enemy::applyGravity(float seconds) {
    movement::tick(body, 0, false, seconds, map->getTileSize(),
        [this](int x, int y) { return map->isSolid(x, y); });
}

void Enemy::patrolGround(float speed, float seconds) {
    if (!body.grounded || seconds <= 0.f) return;
    const int tileSize = map->getTileSize();
    const int feetRow = int(std::floor((body.y + body.height) / tileSize));
    const auto solid = [this](int x, int y) { return map->isSolid(x, y); };
    const auto tryStep = [&](float step) {
        // Comprueba el suelo bajo el pie delantero después del paso.
        const float frontX = body.x + step + (step > 0.f ? body.width - 0.001f : 0.f);
        if (!solid(int(std::floor(frontX / tileSize)), feetRow)) return false;
        const float before = body.x;
        movement::moveAxis(body, step, true, tileSize, solid);
        return body.x != before;
    };
    const float step = (facingLeft ? -speed : speed) * seconds;
    if (!tryStep(step) && tryStep(-step)) facingLeft = !facingLeft;
}

void Enemy::updateSprite(int deltaTime, int anim) {
    if (sprite->animation() != anim) sprite->changeAnimation(anim);
    sprite->update(deltaTime);
    sprite->setPosition(glm::vec2(body.x, body.y) + spriteOffset());
}

void Enemy::kill() {
    if (dying) return;
    dying = true;
    deathMs = 0;
    body.vx = 0.f;
    body.vy = -250.f;  // pequeño salto inicial
    clearProjectiles();
    sprite->changeAnimation(deathAnimation());
}

void Enemy::updateDeath(int deltaTime) {
    deathMs += deltaTime;
    moveWhileDying(deltaTime / 1000.f);
    sprite->update(deltaTime);
    sprite->setPosition(glm::vec2(body.x, body.y) + spriteOffset());
}

// Sin colisiones: sube un poco y cae fuera de la pantalla.
void Enemy::moveWhileDying(float seconds) {
    body.vy += 900.f * seconds;
    body.y += body.vy * seconds;
}
