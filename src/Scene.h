#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <string>
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
#include "Boo.h"
#include "Door.h"
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
    void killAllEnemies();  // tecla K: la puerta se abre al no quedar enemigos
    // El menú y las pantallas usan el mismo shader que la partida.
    ShaderProgram& getShaderProgram() { return texProgram; }

    // Niveles 1..LEVEL_COUNT, en levels/levelNN.txt.
    static const int LEVEL_COUNT = 5;
    static std::string levelFile(int number);
    static bool levelExists(int number);
    // Partida nueva desde la selección de nivel: jugador con sus valores iniciales.
    void startNewGame(int number);
    // Siguiente nivel tras cruzar el portal: conserva vidas y mejoras.
    void startLevel(int number);
    int getLevel() const { return level; }

    // Lo que ha pasado en la partida; Game lo consulta tras cada update.
    enum Result { RUNNING, LEVEL_COMPLETED, GAME_OVER };
    Result getResult() const { return result; }

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
    int aliveEnemies() const;
    void loadLevel();
    void setupDoor();

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
    Texture* booTexture;

    // Objetos escondidos en arbustos (Fire Up, Bomb Up, corazón, estrella).
    std::vector<Item> items;
    Texture* itemTexture;
    Sprite* itemSprite;  // una animación por tipo de objeto

    // Puerta-portal del final: se abre cuando no quedan enemigos.
    Texture* doorTexture;
    Door door;
    bool levelHasDoor = false;

    Hud* hud;
    int level;  // número del nivel cargado (1..LEVEL_COUNT)
    Result result = RUNNING;
    bool timeUp = false;  // se acabó el tiempo: tras la animación de muerte, game over
    int timeLeftMs;
    bool mapEffectsTest = false;
    bool originalEffectsTest = false;
    bool invulnerableMode;  // tecla G

    ShaderProgram texProgram;
    glm::mat4 projection;
    float currentTime;
};
#endif // _SCENE_INCLUDE

