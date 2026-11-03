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

bool Enemy::walk(float speed, float seconds) {
    const float step = (facingLeft ? -speed : speed) * seconds;
    const float before = body.x;
    movement::moveAxis(body, step, true, map->getTileSize(),
        [this](int x, int y) { return map->isSolid(x, y); });
    return std::abs(body.x - before) < std::abs(step) - 0.001f;
}

bool Enemy::floorAhead() const {
    const int tileSize = map->getTileSize();
    const float frontX = facingLeft ? body.x - 1.f : body.x + body.width;
    const int tileX = int(std::floor(frontX / tileSize));
    const int tileY = int(std::floor((body.y + body.height) / tileSize));
    return map->isSolid(tileX, tileY);
}

void Enemy::updateSprite(int deltaTime, int anim) {
    if (sprite->animation() != anim) sprite->changeAnimation(anim);
    sprite->update(deltaTime);
    sprite->setPosition(glm::vec2(body.x, body.y));
}
