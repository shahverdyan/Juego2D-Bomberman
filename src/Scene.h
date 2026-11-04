#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <vector>
#include <glm/glm.hpp>
#include "Texture.h"
#include "Sprite.h"
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "Bomb.h"
#include "Shooter.h"
#include "Walker.h"
#include "Hud.h"
#include "Item.h"

enum BlastType {
    BLAST_CENTER = 0,      // Atlas original: una columna por tipo de pieza
    BLAST_TOP_END,
    BLAST_BOTTOM_END,
    BLAST_LEFT_END,
    BLAST_RIGHT_END,
    BLAST_VERTICAL_MID,
    BLAST_HORIZONTAL_MID,
    BLAST_TYPE_COUNT
};

class Scene {
public:
    Scene();
    ~Scene();

    void init();
    void startMapEffectsTest();
    void startOriginalEffectsTest();
    void runHudTestStep(int step);
    void runBombCollisionTest();
    void update(int deltaTime);
    void render();
    void shutdown();
    void resetPlayer();
    void toggleInvulnerability() { invulnerableMode = !invulnerableMode; }

    // Tiempo límite del nivel. Cambiar aquí (los niveles futuros serán más pequeños).
    static const int LEVEL_TIME_SECONDS = 4 * 60;

private:
    void initShaders();
    void detonate(Bomb& bomb);
    void placeBomb();
    void spawnEnemies();
    void spawnItems();
    void pickUpItems();
    bool touchesFire(const movement::Body& body) const;
    void killEnemiesInBlast();
    void damagePlayer();
    void restartLevel();

    TileMap* map;
    Player* player;
    Texture* bombTexture;

    // Three original frames, anchored to the same centre and base.
    Sprite* bombSprite;

    Texture* explosionTexture;
    Sprite* explosionSprites[BLAST_TYPE_COUNT];

    std::vector<Bomb> bombs;

    // Enemigos de todos los tipos (Shooter, Walker...) tratados como Enemy*.
    // Las texturas se comparten entre todos los enemigos del mismo tipo.
    std::vector<Enemy*> enemies;
    Texture* shooterTexture;
    Texture* shooterBulletTexture;
    Texture* walkerTexture;

    // Objetos escondidos en arbustos (Fire Up, Bomb Up, corazón, estrella).
    std::vector<Item> items;
    Texture* itemTexture;
    Sprite* itemSprite;  // una animación por tipo de objeto

    Hud* hud;
    int level;
    int timeLeftMs;
    bool mapEffectsTest = false;
    bool originalEffectsTest = false;
    bool invulnerableMode;  // tecla G

    ShaderProgram texProgram;
    glm::mat4 projection;
    float currentTime;
};
#endif // _SCENE_INCLUDE

