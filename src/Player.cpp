#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <GL/glew.h>
#include "Player.h"
#include "Audio.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4


// Atlas rick-atlas.png: 7 columnas x 2 filas de 24x24.
// Fila 0 mira a la derecha, fila 1 a la izquierda.
// Columnas: 0 quieto, 1-3 caminar, 4 subiendo, 5 cayendo, 6 muerte (de frente).
enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, JUMP_LEFT, JUMP_RIGHT, FALL_LEFT, FALL_RIGHT, DEATH
};


Player::Player()
{
	sprite = NULL;
	map = NULL;
}

Player::~Player()
{
	if (sprite != NULL)
		delete sprite;
}

void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{

	if (!spritesheet.loadFromFile("images/rick-atlas.png", TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/rick-atlas.png");
    spritesheet.setMinFilter(GL_NEAREST);
    spritesheet.setMagFilter(GL_NEAREST);
	const glm::vec2 cell(1.f / 7.f, 1.f / 2.f);
	sprite = Sprite::createSprite(glm::ivec2(32, 32), cell, &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(9);
	sprite->setAnimationSpeed(DEATH, 1);
	sprite->addKeyframe(DEATH, glm::vec2(6 * cell.x, 0.f));
	for (int side = 0; side < 2; ++side) {
		// side 0 = izquierda (fila 1 del atlas), side 1 = derecha (fila 0).
		const float v = (side == 0) ? cell.y : 0.f;
		sprite->setAnimationSpeed(STAND_LEFT + side, 8);
		sprite->addKeyframe(STAND_LEFT + side, glm::vec2(0.f, v));
		sprite->setAnimationSpeed(MOVE_LEFT + side, 10);
		for (int col = 1; col <= 3; ++col)
			sprite->addKeyframe(MOVE_LEFT + side, glm::vec2(col * cell.x, v));
		sprite->setAnimationSpeed(JUMP_LEFT + side, 8);
		sprite->addKeyframe(JUMP_LEFT + side, glm::vec2(4 * cell.x, v));
		sprite->setAnimationSpeed(FALL_LEFT + side, 8);
		sprite->addKeyframe(FALL_LEFT + side, glm::vec2(5 * cell.x, v));
	}
		
	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + body.x), float(tileMapDispl.y + body.y)));
	
}

bool Player::update(int deltaTime, std::vector<Bomb>& bombs)
{
    health.update(deltaTime);
    starMs = std::max(0, starMs - deltaTime);
    auto& game = Game::instance();
    int direction = int(game.getKey(GLFW_KEY_RIGHT) || game.getKey(GLFW_KEY_D))
                  - int(game.getKey(GLFW_KEY_LEFT) || game.getKey(GLFW_KEY_A));
    const bool placeBomb = game.consumeKey(GLFW_KEY_SPACE);
    bool jump = game.consumeKey(GLFW_KEY_UP);
    jump = game.consumeKey(GLFW_KEY_W) || jump;
    // Respawning inside an existing bomb must also allow one safe exit.
    for (Bomb& bomb : bombs) {
        if (refreshBombExit) bomb.allowOwnerExit(body.x,body.y,body.width,body.height);
        else bomb.updateOwnerExit(body.x,body.y,body.width,body.height);
    }
    refreshBombExit = false;
    const bool wasGrounded = body.grounded;
    movement::tick(body, direction, jump, deltaTime / 1000.f, map->getTileSize(),
        [this,&bombs](int x,int y) {
            if (map->isSolid(x,y)) return true;
            for (const Bomb& bomb : bombs) if (bomb.blocksPlayerTile(x,y)) return true;
            return false;
        });
    for (Bomb& bomb : bombs) bomb.updateOwnerExit(body.x,body.y,body.width,body.height);
    // Ha saltado si estaba en el suelo, pulsó saltar y ahora sube.
    if (jump && wasGrounded && body.vy < 0.f) Audio::instance().play(Audio::JUMP);
    if (direction != 0) facingLeft = direction < 0;
    const int side = facingLeft ? 0 : 1;
    int anim = (direction ? MOVE_LEFT : STAND_LEFT) + side;
    if (!body.grounded) anim = (body.vy < 0.f ? JUMP_LEFT : FALL_LEFT) + side;
    if (sprite->animation() != anim) sprite->changeAnimation(anim);
    sprite->update(deltaTime);
    sprite->setPosition(glm::vec2(body.x, body.y));
    return placeBomb;
}

void Player::render()
{
	// Parpadeo mientras es invulnerable: se dibuja 100 ms sí, 100 ms no.
	if (!dying && !health.visible()) return;
	sprite->render();
}

void Player::startDying()
{
	dying = true;
	deathMs = 0;
	body.vx = body.vy = 0.f;
	sprite->changeAnimation(DEATH);
}

// Sin colisiones: quieto DEATH_PAUSE_MS, luego salta y cae atravesando el suelo.
void Player::updateDeath(int deltaTime)
{
	// Se descarta el Espacio pulsado mientras se muere: si no, se guardaría
	// y la bomba aparecería en el punto de inicio nada más reaparecer.
	Game::instance().consumeKey(GLFW_KEY_SPACE);
	const bool jumpNow = deathMs < DEATH_PAUSE_MS && deathMs + deltaTime >= DEATH_PAUSE_MS;
	deathMs += deltaTime;
	if (jumpNow) body.vy = -420.f;
	if (deathMs >= DEATH_PAUSE_MS) {
		const float seconds = deltaTime / 1000.f;
		body.vy += 1100.f * seconds;
		body.y += body.vy * seconds;
	}
	sprite->update(deltaTime);
	sprite->setPosition(glm::vec2(body.x, body.y));
}

void Player::resetStats()
{
	dying = false;
	health.reset();
	starMs = 0;
	bombCapacity = 1;
	fireRange = Bomb::DEFAULT_FIRE_RANGE;
}

void Player::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2 &pos)
{
	if (dying) sprite->changeAnimation(facingLeft ? STAND_LEFT : STAND_RIGHT);
	dying = false;  // reaparecer termina la animación de muerte
	body.x = pos.x; body.y = pos.y; body.vx = body.vy = 0; body.grounded = false;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + body.x), float(tileMapDispl.y + body.y)));
}




