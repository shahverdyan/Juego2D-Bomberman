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
    BLAST_CENTER = 0,      // Columna 6 (Cruz central)
    BLAST_TOP_END,        // Columna 0 (Punta hacia arriba)
    BLAST_BOTTOM_END,     // Columna 1 (Punta hacia abajo)
    BLAST_LEFT_END,       // Columna 2 (Punta hacia la izquierda)
    BLAST_RIGHT_END,      // Columna 3 (Punta hacia la derecha)
    BLAST_VERTICAL_MID,   // Columna 4 (Cuerpo vertical)
    BLAST_HORIZONTAL_MID, // Columna 5 (Cuerpo horizontal)
    BLAST_TYPE_COUNT
};

class Scene {
public:
    Scene();
    ~Scene();

    void init();
    void update(int deltaTime);
    void render();
    void shutdown();
    void resetPlayer();
    void toggleInvulnerability() { invulnerableMode = !invulnerableMode; }

    // Tiempo límite del nivel. Cambiar aquí (los niveles futuros serán más pequeños).
    static const int LEVEL_TIME_SECONDS = 3 * 60;

private:
    void initShaders();
    void detonate(Bomb& bomb);
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

    // 3 Estados de la bomba
    Sprite* bombSprite;        // Normal
    Sprite* mediumBombSprite;  // Estado medio (al pasar 1.5s)
    Sprite* largeBombSprite;   // Estado grande (últimos 0.5s)

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
    bool invulnerableMode;  // tecla G

    ShaderProgram texProgram;
    glm::mat4 projection;
    float currentTime;
};
#endif // _SCENE_INCLUDE

