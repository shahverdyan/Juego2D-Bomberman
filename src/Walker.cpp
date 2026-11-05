#include "Walker.h"

// koopa-atlas.png (tools/build_koopa.py): 8 columnas x 3 filas de 32x32.
// Fila 0 mira a la izquierda y fila 1 a la derecha; las columnas son los pasos.
// Fila 2, muerte: agachado, dentro del caparazón y caparazón boca abajo.
enum WalkerAnims { WALK_LEFT, WALK_RIGHT, DEATH };
static const int WALK_FRAMES = 8;

Walker::Walker(const glm::vec2& position, bool facingLeft)
    : Enemy(position, WIDTH, HEIGHT, facingLeft) {}

void Walker::init(Texture* spritesheet, ShaderProgram& program) {
    const float cellW = 1.f / WALK_FRAMES, cellH = 1.f / 3.f;
    sprite = Sprite::createSprite(glm::vec2(WIDTH, HEIGHT), glm::vec2(cellW, cellH),
        spritesheet, &program);
    sprite->setNumberAnimations(3);
    for (int side = 0; side < 2; ++side) {
        sprite->setAnimationSpeed(WALK_LEFT + side, 10);
        for (int step = 0; step < WALK_FRAMES; ++step)
            sprite->addKeyframe(WALK_LEFT + side, glm::vec2(step * cellW, side * cellH));
    }
    // Muerte a 8 fps: se mete en el caparazón (250 ms) y el caparazón queda boca
    // abajo el resto de DEATH_MS, sin que la animación llegue a repetirse.
    sprite->setAnimationSpeed(DEATH, 8);
    sprite->addKeyframe(DEATH, glm::vec2(0.f, 2 * cellH));
    sprite->addKeyframe(DEATH, glm::vec2(cellW, 2 * cellH));
    for (int i = 0; i < 6; ++i)
        sprite->addKeyframe(DEATH, glm::vec2(2 * cellW, 2 * cellH));
    sprite->changeAnimation(facingLeft ? WALK_LEFT : WALK_RIGHT);
    sprite->setPosition(glm::vec2(body.x, body.y));
}

// El jugador no influye en su comportamiento.
void Walker::update(int deltaTime, const movement::Body&, bool) {
    const float seconds = deltaTime / 1000.f;
    if (walk(WALK_SPEED, seconds)) facingLeft = !facingLeft;
    applyGravity(seconds);
    updateSprite(deltaTime, facingLeft ? WALK_LEFT : WALK_RIGHT);
}

int Walker::deathAnimation() const {
    return DEATH;
}
