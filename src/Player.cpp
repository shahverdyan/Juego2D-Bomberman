#include <cmath>
#include <iostream>
#include <stdexcept>
#include <GL/glew.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4


enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT
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

	if (!spritesheet.loadFromFile("images/bub.png", TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/bub.png");
    spritesheet.setMinFilter(GL_NEAREST);
    spritesheet.setMagFilter(GL_NEAREST);
	sprite = Sprite::createSprite(glm::ivec2(32, 32), glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(4);
	
		sprite->setAnimationSpeed(STAND_LEFT, 8);
		sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.f));
		
		sprite->setAnimationSpeed(STAND_RIGHT, 8);
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.25f, 0.f));
		
		sprite->setAnimationSpeed(MOVE_LEFT, 8);
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.f));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.25f));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.5f));
		
		sprite->setAnimationSpeed(MOVE_RIGHT, 8);
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.f));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.25f));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.5f));
		
	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + body.x), float(tileMapDispl.y + body.y)));
	
}

void Player::update(int deltaTime)
{
    auto& game = Game::instance();
    int direction = int(game.getKey(GLFW_KEY_RIGHT) || game.getKey(GLFW_KEY_D))
                  - int(game.getKey(GLFW_KEY_LEFT) || game.getKey(GLFW_KEY_A));
    bool jump = game.consumeKey(GLFW_KEY_SPACE);
    jump = game.consumeKey(GLFW_KEY_UP) || jump;
    jump = game.consumeKey(GLFW_KEY_W) || jump;
    movement::tick(body, direction, jump, deltaTime / 1000.f, map->getTileSize(),
        [this](int x, int y) { return map->isSolid(x, y); });
    if (direction != 0) facingLeft = direction < 0;
    int anim = direction ? (facingLeft ? MOVE_LEFT : MOVE_RIGHT) : (facingLeft ? STAND_LEFT : STAND_RIGHT);
    if (sprite->animation() != anim) sprite->changeAnimation(anim);
    sprite->update(deltaTime);
    sprite->setPosition(glm::vec2(body.x, body.y));
}

void Player::render()
{
	sprite->render();
}

void Player::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2 &pos)
{
	body.x = pos.x; body.y = pos.y; body.vx = body.vy = 0; body.grounded = false;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + body.x), float(tileMapDispl.y + body.y)));
}




