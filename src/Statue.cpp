#include "Statue.h"
#include "Audio.h"

// gargoyle-atlas.png (tools/build_boss.py): 3 fotogramas de 16x16 en una fila,
// dibujados a escala 2 (32x32). Fotograma 0: estatua quieta; 1: rota (muerte).
enum StatueAnims { STILL, BROKEN };
static const float SPRITE_SIZE = 32.f;

Statue::Statue(const glm::vec2& position)
    : Enemy(position, WIDTH, HEIGHT, false) {}

void Statue::init(Texture* spritesheet, ShaderProgram& program) {
    const float cellW = 1.f / 3.f;
    sprite = Sprite::createSprite(glm::vec2(SPRITE_SIZE), glm::vec2(cellW, 1.f), spritesheet, &program);
    sprite->setNumberAnimations(2);
    sprite->setAnimationSpeed(STILL, 1);
    sprite->addKeyframe(STILL, glm::vec2(0.f, 0.f));
    sprite->setAnimationSpeed(BROKEN, 1);
    sprite->addKeyframe(BROKEN, glm::vec2(cellW, 0.f));
    sprite->changeAnimation(STILL);
    sprite->setPosition(glm::vec2(body.x, body.y) + spriteOffset());
}

void Statue::update(int deltaTime, const movement::Body&, bool) {
    ageMs += deltaTime;
    if (ageMs > WARN_MS) {
        // Cae con la misma gravedad que el jugador y se para en lo sólido.
        applyGravity(deltaTime / 1000.f);
        if (body.grounded && landedMs < 0) {
            landedMs = 0;
            Audio::instance().play(Audio::STATUE_LAND);
        }
    }
    if (landedMs >= 0) {
        landedMs += deltaTime;
        if (landedMs >= STAND_MS) kill();  // se rompe sola
    }
    updateSprite(deltaTime, STILL);
}

void Statue::render() {
    // Parpadea mientras avisa, antes de caer.
    if (ageMs > WARN_MS || (ageMs / 100) % 2 == 0) Enemy::render();
}

bool Statue::touches(const movement::Body& other) const {
    const bool falling = ageMs > WARN_MS && landedMs < 0;
    return falling && Enemy::touches(other);
}

int Statue::deathAnimation() const {
    return BROKEN;
}

// El dibujo de 32x32 se centra horizontalmente y se apoya en la base de la hitbox.
glm::vec2 Statue::spriteOffset() const {
    return glm::vec2((WIDTH - SPRITE_SIZE) / 2.f, HEIGHT - SPRITE_SIZE);
}
