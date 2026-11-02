#pragma once
#include "Sprite.h"
#include "TileMap.h"
#include "Movement.h"
class Player {
public:
    Player();
    ~Player();
    void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
    void update(int deltaTime);
    void render();
    void setTileMap(TileMap* tileMap);
    void setPosition(const glm::vec2& pos);
    glm::vec2 getPosition() const { return glm::vec2(body.x, body.y); }
private:
    movement::Body body;
    bool facingLeft = false;
    glm::ivec2 tileMapDispl;
    Texture spritesheet;
    Sprite* sprite;
    TileMap* map;
};
