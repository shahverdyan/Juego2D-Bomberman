#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "Boss.h"
#include "Audio.h"
#include "Statue.h"

// boss-atlas.png (tools/build_boss.py): 4 fotogramas de 32x32 dibujados a escala 2.
// 0-1 reposo, 2 alas levantadas (ataque), 3 herido.
enum BossAnims { IDLE, WINGS_UP, HURT };
// boss-fireball.png: 6 fotogramas de 16x16; 0-2 bola latiendo, 3-5 estallido.
// Cada fotograma es una animación de un solo keyframe: se elige según el tiempo.
static const float SPRITE_SIZE = 64.f;
static const float FIREBALL_SIZE = 32.f, FIREBALL_HITBOX = 20.f;  // hitbox centrada en el dibujo
static const float BIG_SIZE = 56.f, BIG_HITBOX = 38.f;
static const int FIREBALL_FRAMES = 6, BURST_MS = 300;
static const int BOUNCE_LIFE_MS = 7000;  // lo que duran las bolas grandes rebotando

// Duración de cada fase (ms). El teletransporte es VANISH + APPEAR.
static const int FLOAT_MS = 2500, VANISH_MS = 500, APPEAR_MS = 500, ATTACK_MS = 900;
static const float TELEPORT_MIN_DISTANCE = 120.f;  // reaparece al menos a esta distancia del jugador

static float random01() {
    return std::rand() / float(RAND_MAX);
}

Boss::Boss(const glm::vec2& position, float minX, float maxX, float minY, float maxY)
    : Enemy(position, WIDTH, HEIGHT, true), minX(minX), maxX(maxX), minY(minY), maxY(maxY),
      target(position) {}

Boss::~Boss() {
    delete fireballSprite;
    delete bigFireballSprite;
    for (Enemy* statue : newStatues) delete statue;
}

void Boss::init(Texture* bossTexture, Texture* fireballTexture, Texture* statues,
                ShaderProgram& shaderProgram) {
    program = &shaderProgram;
    statueTexture = statues;

    const float cellW = 1.f / 4.f;
    sprite = Sprite::createSprite(glm::vec2(SPRITE_SIZE), glm::vec2(cellW, 1.f), bossTexture, program);
    sprite->setNumberAnimations(3);
    sprite->setAnimationSpeed(IDLE, 4);
    sprite->addKeyframe(IDLE, glm::vec2(0.f, 0.f));
    sprite->addKeyframe(IDLE, glm::vec2(cellW, 0.f));
    sprite->setAnimationSpeed(WINGS_UP, 1);
    sprite->addKeyframe(WINGS_UP, glm::vec2(2 * cellW, 0.f));
    sprite->setAnimationSpeed(HURT, 1);
    sprite->addKeyframe(HURT, glm::vec2(3 * cellW, 0.f));
    sprite->changeAnimation(IDLE);
    sprite->setPosition(glm::vec2(body.x, body.y) + spriteOffset());

    // La misma textura sirve para las bolas normales y las grandes (otro tamaño de dibujo).
    const float fireCellW = 1.f / FIREBALL_FRAMES;
    for (Sprite** fire : { &fireballSprite, &bigFireballSprite }) {
        const float size = fire == &fireballSprite ? FIREBALL_SIZE : BIG_SIZE;
        *fire = Sprite::createSprite(glm::vec2(size), glm::vec2(fireCellW, 1.f), fireballTexture, program);
        (*fire)->setNumberAnimations(FIREBALL_FRAMES);
        for (int frame = 0; frame < FIREBALL_FRAMES; ++frame) {
            (*fire)->setAnimationSpeed(frame, 1);
            (*fire)->addKeyframe(frame, glm::vec2(frame * fireCellW, 0.f));
        }
    }
}

void Boss::startPhase(Phase newPhase) {
    phase = newPhase;
    phaseMs = 0;
    attacked = false;
}

glm::vec2 Boss::randomPosition() const {
    return glm::vec2(minX + random01() * (maxX - minX), minY + random01() * (maxY - minY));
}

bool Boss::hidden() const {
    return (phase == VANISH && phaseMs >= VANISH_MS / 2) || (phase == APPEAR && phaseMs < APPEAR_MS / 2);
}

void Boss::update(int deltaTime, const movement::Body& player, bool) {
    const float seconds = deltaTime / 1000.f;
    timeMs += deltaTime;
    phaseMs += deltaTime;
    hurtMs = std::max(0, hurtMs - deltaTime);

    switch (phase) {
    case FLOAT: {
        // Flota despacio hacia el objetivo; al llegar elige otro al azar.
        const glm::vec2 toTarget = target - glm::vec2(body.x, body.y);
        const float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
        const float step = FLOAT_SPEED * seconds;
        if (distance <= step) target = randomPosition();
        else {
            body.x += toTarget.x / distance * step;
            body.y += toTarget.y / distance * step;
        }
        if (phaseMs >= FLOAT_MS) {
            // Una vez sí y otra no, se teletransporta antes de atacar.
            startPhase(cycle % 2 == 1 ? VANISH : ATTACK);
            if (phase == VANISH) Audio::instance().play(Audio::BOSS_TELEPORT);
            ++cycle;
        }
        break;
    }
    case VANISH:
        if (phaseMs >= VANISH_MS) {
            // Reaparece en un sitio al azar, pero no encima del jugador.
            glm::vec2 position;
            for (int tries = 0; tries < 20; ++tries) {
                position = randomPosition();
                const float dx = (position.x + WIDTH / 2.f) - (player.x + player.width / 2.f);
                const float dy = (position.y + HEIGHT / 2.f) - (player.y + player.height / 2.f);
                if (std::sqrt(dx * dx + dy * dy) >= TELEPORT_MIN_DISTANCE) break;
            }
            body.x = position.x;
            body.y = position.y;
            target = position;
            startPhase(APPEAR);
            Audio::instance().play(Audio::BOSS_TELEPORT);
        }
        break;
    case APPEAR:
        if (phaseMs >= APPEAR_MS) startPhase(ATTACK);
        break;
    case ATTACK:
        // Quieto con las alas levantadas; el ataque sale a mitad de la fase.
        if (!attacked && phaseMs >= ATTACK_MS / 2) {
            attack(player);
            attacked = true;
        }
        if (phaseMs >= ATTACK_MS) startPhase(FLOAT);
        break;
    }

    updateFireballs(deltaTime);

    int anim = IDLE;
    if (hurtMs > HURT_MS - 600) anim = HURT;  // cara de dolor justo después del golpe
    else if (phase == ATTACK) anim = WINGS_UP;
    updateSprite(deltaTime, anim);
}

void Boss::attack(const movement::Body& player) {
    const float centerX = body.x + body.width / 2.f;
    const float centerY = body.y + body.height / 2.f;
    const float diagonal = 0.7071f;  // cos(45º) = sin(45º)

    if (nextAttack != FALLING_STATUES) Audio::instance().play(Audio::BOSS_SHOOT);
    switch (nextAttack) {
    case BOUNCING_BALLS:
        // Dos bolas grandes en diagonal hacia arriba, una a cada lado.
        for (float dirX : { -1.f, 1.f }) {
            fireballs.push_back({ centerX - BIG_SIZE / 2.f, centerY - BIG_SIZE / 2.f,
                dirX * diagonal * BOUNCE_SPEED, -diagonal * BOUNCE_SPEED, -1, true, 0 });
        }
        break;
    case DIAGONAL_SHOTS:
        // Cuatro bolas en forma de X.
        for (float dirX : { -1.f, 1.f })
            for (float dirY : { -1.f, 1.f })
                fireballs.push_back({ centerX - FIREBALL_SIZE / 2.f, centerY - FIREBALL_SIZE / 2.f,
                    dirX * diagonal * FIREBALL_SPEED, dirY * diagonal * FIREBALL_SPEED, -1, false, 0 });
        break;
    case FALLING_STATUES: {
        // Una estatua pegada al techo, encima del jugador.
        // La recoge la Scene con takeSpawnedEnemies().
        const float minStatueX = minX, maxStatueX = maxX + WIDTH - Statue::WIDTH;
        float x = player.x + player.width / 2.f - Statue::WIDTH / 2.f;
        x = std::max(minStatueX, std::min(maxStatueX, x));
        Statue* statue = new Statue(glm::vec2(x, float(map->getTileSize())));
        statue->init(statueTexture, *program);
        newStatues.push_back(statue);
        break;
    }
    default:
        break;
    }
    nextAttack = Attack((nextAttack + 1) % ATTACK_COUNT);
}

void Boss::updateFireballs(int deltaTime) {
    const float seconds = deltaTime / 1000.f;
    const int tileSize = map->getTileSize();
    // ¿Hay terreno sólido en este punto?
    auto solidAt = [&](float x, float y) {
        return map->isSolid(int(std::floor(x / tileSize)), int(std::floor(y / tileSize)));
    };

    for (auto it = fireballs.begin(); it != fireballs.end();) {
        if (it->burstMs >= 0) {
            // Estallando: no se mueve y desaparece al acabar la animación.
            it->burstMs += deltaTime;
            if (it->burstMs >= BURST_MS) { it = fireballs.erase(it); continue; }
        } else if (it->big) {
            // Rebota: si el borde de la hitbox entraría en terreno sólido, se invierte
            // la velocidad en ese eje. Cada eje se comprueba por separado.
            const float half = BIG_SIZE / 2.f, radius = BIG_HITBOX / 2.f;
            const float cx = it->x + half, cy = it->y + half;
            const float nextX = cx + it->vx * seconds, nextY = cy + it->vy * seconds;
            if (solidAt(nextX + (it->vx > 0 ? radius : -radius), cy)) it->vx = -it->vx;
            else it->x += it->vx * seconds;
            if (solidAt(cx, nextY + (it->vy > 0 ? radius : -radius))) it->vy = -it->vy;
            else it->y += it->vy * seconds;
            it->lifeMs += deltaTime;
            if (it->lifeMs >= BOUNCE_LIFE_MS) it->burstMs = 0;
        } else {
            it->x += it->vx * seconds;
            it->y += it->vy * seconds;
            // Estalla al chocar con terreno sólido (o al salir del mapa).
            if (solidAt(it->x + FIREBALL_SIZE / 2.f, it->y + FIREBALL_SIZE / 2.f)) it->burstMs = 0;
        }
        ++it;
    }
}

bool Boss::touches(const movement::Body& other) const {
    return !hidden() && Enemy::touches(other);
}

bool Boss::hitsPlayer(const movement::Body& player) {
    for (Fireball& fireball : fireballs) {
        if (fireball.burstMs >= 0) continue;
        const float size = fireball.big ? BIG_SIZE : FIREBALL_SIZE;
        const float hitbox = fireball.big ? BIG_HITBOX : FIREBALL_HITBOX;
        const float margin = (size - hitbox) / 2.f;
        const float x = fireball.x + margin, y = fireball.y + margin;
        if (x < player.x + player.width && x + hitbox > player.x &&
            y < player.y + player.height && y + hitbox > player.y) {
            fireball.burstMs = 0;  // la bola estalla sobre el jugador
            return true;
        }
    }
    return false;
}

void Boss::clearProjectiles() {
    fireballs.clear();
}

void Boss::hitByExplosion() {
    if (dying || hurtMs > 0 || hidden()) return;
    --health;
    Audio::instance().play(Audio::BOSS_HURT);
    if (health <= 0) kill();
    else hurtMs = HURT_MS;
}

void Boss::takeSpawnedEnemies(std::vector<Enemy*>& out) {
    out.insert(out.end(), newStatues.begin(), newStatues.end());
    newStatues.clear();
}

void Boss::render() {
    // Parpadeo rápido al teletransportarse; normal tras un golpe y mientras muere.
    bool visible = true;
    if (dying) visible = (deathMs / 80) % 2 == 0;
    else if (phase == VANISH || phase == APPEAR) visible = !hidden() && (phaseMs / 50) % 2 == 0;
    else if (hurtMs > 0) visible = (hurtMs / 80) % 2 == 0;
    if (visible) Enemy::render();

    for (const Fireball& fireball : fireballs) {
        const int frame = fireball.burstMs < 0 ? (timeMs / 100) % 3
                                               : 3 + std::min(2, fireball.burstMs / 100);
        Sprite* fire = fireball.big ? bigFireballSprite : fireballSprite;
        fire->changeAnimation(frame);
        fire->setPosition(glm::vec2(fireball.x, fireball.y));
        fire->render();
    }
}

int Boss::deathAnimation() const {
    return HURT;
}

void Boss::moveWhileDying(float seconds) {
    body.y += 40.f * seconds;
}

// El dibujo de 64x64 se centra horizontalmente y se apoya en la base de la hitbox.
glm::vec2 Boss::spriteOffset() const {
    return glm::vec2((WIDTH - SPRITE_SIZE) / 2.f, HEIGHT - SPRITE_SIZE);
}
