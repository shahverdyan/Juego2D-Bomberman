#pragma once
#include <cstddef>
#include <vector>
#include "Bomb.h"
#include "Sprite.h"
#include "TileMap.h"
#include "Movement.h"
class Player {
public:
    Player();
    ~Player();
    void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
    bool update(int deltaTime, const std::vector<Bomb>& bombs);
    void render();
    void setTileMap(TileMap* tileMap);
    void setPosition(const glm::vec2& pos);
    glm::vec2 getPosition() const { return glm::vec2(body.x, body.y); }
    bool canPlaceBomb(std::size_t activeBombCount) const { return activeBombCount < bombCapacity; }
    int getFireRange() const { return fireRange; }
    void increaseBombCapacity() { ++bombCapacity; }
    void increaseFireRange() { ++fireRange; }
private:
    movement::Body body;
    bool facingLeft = false;
    std::size_t bombCapacity = 1;
    int fireRange = Bomb::DEFAULT_FIRE_RANGE;
    glm::ivec2 tileMapDispl;
    Texture spritesheet;
    Sprite* sprite;
    TileMap* map;
};
