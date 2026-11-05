#include "Door.h"
#include "Audio.h"

// portal-atlas.png (tools/build_portal.py): 7 fotogramas de 28x32 en una fila.
// 0 cerrada, 1-4 abriéndose, 5-6 portal abierto girando.
enum DoorAnims { ANIM_CLOSED, ANIM_OPENING, ANIM_OPEN };
static const int FRAME_COUNT = 7;

Door::~Door() {
    delete sprite;
}

void Door::init(Texture* spritesheet, ShaderProgram& program, const glm::vec2& bottomCenter) {
    state = CLOSED;
    openingMs = 0;
    position = glm::vec2(bottomCenter.x - WIDTH / 2.f, bottomCenter.y - HEIGHT);

    const float cellW = 1.f / FRAME_COUNT;
    delete sprite;
    sprite = Sprite::createSprite(glm::vec2(WIDTH, HEIGHT), glm::vec2(cellW, 1.f), spritesheet, &program);
    sprite->setNumberAnimations(3);
    sprite->setAnimationSpeed(ANIM_CLOSED, 1);
    sprite->addKeyframe(ANIM_CLOSED, glm::vec2(0.f, 0.f));
    // Dura OPENING_MS: al acabar el cuarto fotograma pasa a ANIM_OPEN.
    sprite->setAnimationSpeed(ANIM_OPENING, OPENING_FPS);
    for (int frame = 1; frame <= 4; ++frame)
        sprite->addKeyframe(ANIM_OPENING, glm::vec2(frame * cellW, 0.f));
    sprite->setAnimationSpeed(ANIM_OPEN, 4);
    for (int frame = 5; frame <= 6; ++frame)
        sprite->addKeyframe(ANIM_OPEN, glm::vec2(frame * cellW, 0.f));
    sprite->changeAnimation(ANIM_CLOSED);
    sprite->setPosition(position);
}

void Door::open() {
    if (state != CLOSED) return;
    Audio::instance().play(Audio::DOOR_OPEN);
    state = OPENING;
    openingMs = 0;
    sprite->changeAnimation(ANIM_OPENING);
}

void Door::update(int deltaTime) {
    if (state == OPENING) {
        openingMs += deltaTime;
        if (openingMs >= OPENING_MS) {
            state = OPEN;
            sprite->changeAnimation(ANIM_OPEN);
        }
    }
    sprite->update(deltaTime);
}

void Door::render() {
    sprite->render();
}

bool Door::contains(const movement::Body& body) const {
    const float centerX = body.x + body.width / 2.f;
    const float centerY = body.y + body.height / 2.f;
    return centerX >= position.x && centerX <= position.x + WIDTH &&
        centerY >= position.y && centerY <= position.y + HEIGHT;
}
