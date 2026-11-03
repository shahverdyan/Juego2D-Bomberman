#include <algorithm>
#include <cmath>
#include "Shooter.h"

// Atlas shooter-atlas.png (tools/build_shooter.py): 8 columnas x 2 filas de 32x48.
// Fila 0 mira a la izquierda, fila 1 a la derecha.
// Columnas: 0-3 caminar, 4 alerta, 6-7 disparo.
enum ShooterAnims {
    WALK_LEFT, WALK_RIGHT, ALERT_LEFT, ALERT_RIGHT, SHOOT_LEFT, SHOOT_RIGHT
};

Shooter::Shooter(const glm::vec2& position, bool facingLeft) : facingLeft(facingLeft) {
    body.x = position.x;
    body.y = position.y;
    body.width = WIDTH;
    body.height = HEIGHT;
}

Shooter::~Shooter() {
    delete sprite;
    delete bulletSprite;
}

void Shooter::init(Texture* spritesheet, Texture* bulletTexture, ShaderProgram& program) {
    const glm::vec2 cell(1.f / 8.f, 1.f / 2.f);
    sprite = Sprite::createSprite(glm::vec2(WIDTH, HEIGHT), cell, spritesheet, &program);
    sprite->setNumberAnimations(6);
    for (int row = 0; row < 2; ++row) {
        const float v = row * cell.y;
        sprite->setAnimationSpeed(WALK_LEFT + row, 6);
        for (int col = 0; col < 4; ++col)
            sprite->addKeyframe(WALK_LEFT + row, glm::vec2(col * cell.x, v));
        sprite->setAnimationSpeed(ALERT_LEFT + row, 1);
        sprite->addKeyframe(ALERT_LEFT + row, glm::vec2(4 * cell.x, v));
        sprite->setAnimationSpeed(SHOOT_LEFT + row, 8);
        sprite->addKeyframe(SHOOT_LEFT + row, glm::vec2(6 * cell.x, v));
        sprite->addKeyframe(SHOOT_LEFT + row, glm::vec2(7 * cell.x, v));
    }
    sprite->changeAnimation(facingLeft ? WALK_LEFT : WALK_RIGHT);
    sprite->setPosition(glm::vec2(body.x, body.y));

    bulletSprite = Sprite::createSprite(glm::vec2(BULLET_SIZE, BULLET_SIZE),
        glm::vec2(1.f, 1.f), bulletTexture, &program);
    bulletSprite->setNumberAnimations(1);
    bulletSprite->setAnimationSpeed(0, 1);
    bulletSprite->addKeyframe(0, glm::vec2(0.f, 0.f));
    bulletSprite->changeAnimation(0);
}

void Shooter::update(int deltaTime, const movement::Body& player) {
    const float seconds = deltaTime / 1000.f;
    seesPlayer = canSee(player);

    if (seesPlayer) {
        // Quieto, apuntando: dispara cuando el temporizador llega a 0.
        shootTimerMs -= deltaTime;
        if (shootTimerMs <= 0) {
            fire();
            shootTimerMs = COOLDOWN_MS;
            fireAnimMs = FIRE_ANIM_MS;
        }
    } else {
        // Sin ver al jugador el temporizador baja, pero nunca por debajo del
        // tiempo de reacción: al volver a verlo siempre tarda un poco en disparar.
        shootTimerMs = std::max(shootTimerMs - deltaTime, int(REACTION_MS));
        patrol(seconds);
    }

    // Gravedad y colisión vertical con la misma física que el jugador (sin salto).
    movement::tick(body, 0, false, seconds, map->getTileSize(),
        [this](int x, int y) { return map->isSolid(x, y); });

    fireAnimMs = std::max(0, fireAnimMs - deltaTime);
    updateBullets(seconds);

    const int side = facingLeft ? 0 : 1;
    int anim = WALK_LEFT + side;
    if (fireAnimMs > 0) anim = SHOOT_LEFT + side;
    else if (seesPlayer) anim = ALERT_LEFT + side;
    if (sprite->animation() != anim) sprite->changeAnimation(anim);
    sprite->update(deltaTime);
    sprite->setPosition(glm::vec2(body.x, body.y));
}

// Ve al jugador si: (1) la línea de la bala cruza su cuerpo, (2) está delante
// y a menos de VISION_RANGE, y (3) no hay tiles sólidos entre ambos.
bool Shooter::canSee(const movement::Body& player) const {
    const float bulletTop = body.y + MUZZLE_Y;
    if (player.y >= bulletTop + BULLET_SIZE || player.y + player.height <= bulletTop)
        return false;

    const float myCenter = body.x + body.width / 2.f;
    const float playerCenter = player.x + player.width / 2.f;
    const float dx = playerCenter - myCenter;
    const bool inFront = facingLeft ? dx < 0.f : dx > 0.f;
    if (!inFront || std::abs(dx) > VISION_RANGE) return false;

    const int tileSize = map->getTileSize();
    const int row = int(std::floor((bulletTop + BULLET_SIZE / 2.f) / tileSize));
    int from = int(std::floor(myCenter / tileSize));
    int to = int(std::floor(playerCenter / tileSize));
    if (from > to) std::swap(from, to);
    for (int x = from; x <= to; ++x)
        if (map->isSolid(x, row)) return false;
    return true;
}

// ¿Hay suelo justo delante de los pies? Evita caerse de la plataforma.
bool Shooter::floorAhead() const {
    const int tileSize = map->getTileSize();
    const float frontX = facingLeft ? body.x - 1.f : body.x + body.width;
    const int tileX = int(std::floor(frontX / tileSize));
    const int tileY = int(std::floor((body.y + body.height) / tileSize));
    return map->isSolid(tileX, tileY);
}

// Camina hacia donde mira y se da la vuelta al llegar a un borde o a una pared.
void Shooter::patrol(float seconds) {
    if (body.grounded && !floorAhead()) facingLeft = !facingLeft;

    const float step = (facingLeft ? -WALK_SPEED : WALK_SPEED) * seconds;
    const float before = body.x;
    movement::moveAxis(body, step, true, map->getTileSize(),
        [this](int x, int y) { return map->isSolid(x, y); });
    if (std::abs(body.x - before) < std::abs(step) - 0.001f) facingLeft = !facingLeft;
}

// La bala sale desde la boca, justo delante del cuerpo.
void Shooter::fire() {
    Bullet bullet;
    bullet.x = facingLeft ? body.x - BULLET_SIZE : body.x + body.width;
    bullet.y = body.y + MUZZLE_Y;
    bullet.vx = facingLeft ? -BULLET_SPEED : BULLET_SPEED;
    bullets.push_back(bullet);
}

// Las balas avanzan en línea recta y desaparecen al tocar un tile sólido.
void Shooter::updateBullets(float seconds) {
    const int tileSize = map->getTileSize();
    for (Bullet& bullet : bullets) bullet.x += bullet.vx * seconds;
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [this, tileSize](const Bullet& bullet) {
            const int tileX = int(std::floor((bullet.x + BULLET_SIZE / 2.f) / tileSize));
            const int tileY = int(std::floor((bullet.y + BULLET_SIZE / 2.f) / tileSize));
            return map->isSolid(tileX, tileY);
        }), bullets.end());
}

bool Shooter::hitsPlayer(const movement::Body& player) {
    for (auto it = bullets.begin(); it != bullets.end(); ++it) {
        const bool overlap = it->x < player.x + player.width && it->x + BULLET_SIZE > player.x &&
            it->y < player.y + player.height && it->y + BULLET_SIZE > player.y;
        if (overlap) {
            bullets.erase(it);
            return true;
        }
    }
    return false;
}

void Shooter::render() {
    sprite->render();
    for (const Bullet& bullet : bullets) {
        bulletSprite->setPosition(glm::vec2(bullet.x, bullet.y));
        bulletSprite->render();
    }
}
