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
    bool isFacingLeft() const { return facingLeft; }
    bool canPlaceBomb(std::size_t activeBombCount) const { return activeBombCount < bombCapacity; }
    int getFireRange() const { return fireRange; }
    std::size_t getBombCapacity() const { return bombCapacity; }
    static constexpr std::size_t MAX_BOMB_CAPACITY = 4;
    static constexpr int MAX_FIRE_RANGE = 4;
    void increaseBombCapacity() { if (bombCapacity < MAX_BOMB_CAPACITY) ++bombCapacity; }
    void increaseFireRange() { if (fireRange < MAX_FIRE_RANGE) ++fireRange; }

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

    // Muerte, como en Mario: cara de susto quieto, salto y caída fuera de la
    // pantalla. Mientras dura, la Scene llama a updateDeath() en vez de update().
    static const int DEATH_PAUSE_MS = 500;  // quieto antes del salto
    static const int DEATH_MS = 1800;       // duración total
    void startDying();
    void updateDeath(int deltaTime);
    bool isDying() const { return dying; }
    bool deathFinished() const { return dying && deathMs >= DEATH_MS; }
private:
    bool dying = false;
    int deathMs = 0;
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
