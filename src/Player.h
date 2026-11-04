#pragma once
#include <cstddef>
#include <vector>
#include "Bomb.h"
#include "Sprite.h"
#include "TileMap.h"
#include "Movement.h"
#include "PlayerHealth.h"
class Player {
public:
    Player();
    ~Player();
    void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
    bool update(int deltaTime, std::vector<Bomb>& bombs);
    void render();
    void setTileMap(TileMap* tileMap);
    void setPosition(const glm::vec2& pos);
    glm::vec2 getPosition() const { return glm::vec2(body.x, body.y); }
    const movement::Body& getBody() const { return body; }
    bool canPlaceBomb(std::size_t activeBombCount) const { return activeBombCount < bombCapacity; }
    int getFireRange() const { return fireRange; }
    std::size_t getBombCapacity() const { return bombCapacity; }
    void increaseBombCapacity() { ++bombCapacity; }
    void increaseFireRange() { ++fireRange; }

    static const int MAX_LIVES = PlayerHealth::MAX_LIVES;
    int getLives() const { return health.lives; }
    int getSpareLives() const { return health.spareLives(); }
    bool hasGoldHeart() const { return health.goldHeart; }
    bool canBeHurt() const { return health.canBeHurt() && starMs == 0; }
    PlayerHealth::Damage takeHit() {
        return canBeHurt() ? health.takeHit() : PlayerHealth::Damage::Ignored;
    }
    void collectGoldHeart() { health.collectGoldHeart(); }

    // Objeto estrella: invulnerable durante STAR_MS (sin parpadeo).
    static const int STAR_MS = 10000;
    void startStar() { starMs = STAR_MS; }
    int getStarMs() const { return starMs; }
    // Vuelve a los valores iniciales (al reiniciar el nivel tras perder todas las vidas).
    void resetStats();
private:
    movement::Body body;
    PlayerHealth health;
    int starMs = 0;
    bool facingLeft = false;
    bool refreshBombExit = true;
    std::size_t bombCapacity = 1;
    int fireRange = Bomb::DEFAULT_FIRE_RANGE;
    glm::ivec2 tileMapDispl;
    Texture spritesheet;
    Sprite* sprite;
    TileMap* map;
};
