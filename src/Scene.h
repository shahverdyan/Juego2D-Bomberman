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

private:
    void initShaders();
    void detonate(Bomb& bomb);

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
    ShaderProgram texProgram;
    glm::mat4 projection;
    float currentTime;
};
#endif // _SCENE_INCLUDE

