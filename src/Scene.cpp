#include <iostream>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"

Scene::Scene() {
    map = NULL;
    player = NULL;
    bombTexture = NULL;
    bombSprite = NULL;
    mediumBombSprite = NULL;
    largeBombSprite = NULL;
    explosionTexture = NULL;
    for (int i = 0; i < BLAST_TYPE_COUNT; ++i) {
        explosionSprites[i] = NULL;
    }
    shooterTexture = NULL;
    shooterBulletTexture = NULL;
    walkerTexture = NULL;
    itemTexture = NULL;
    itemSprite = NULL;
    hud = NULL;
    level = 1;
    timeLeftMs = LEVEL_TIME_SECONDS * 1000;
    invulnerableMode = false;
}

Scene::~Scene() {}

void Scene::shutdown() {
    delete player; player = NULL;
    delete bombSprite; bombSprite = NULL;
    delete mediumBombSprite; mediumBombSprite = NULL;
    delete largeBombSprite; largeBombSprite = NULL;
    delete bombTexture; bombTexture = NULL;

    for (int i = 0; i < BLAST_TYPE_COUNT; ++i) {
        delete explosionSprites[i];
        explosionSprites[i] = NULL;
    }

    delete explosionTexture; explosionTexture = NULL;

    for (Enemy* enemy : enemies) delete enemy;
    enemies.clear();
    delete shooterTexture; shooterTexture = NULL;
    delete shooterBulletTexture; shooterBulletTexture = NULL;
    delete walkerTexture; walkerTexture = NULL;

    delete itemSprite; itemSprite = NULL;
    delete itemTexture; itemTexture = NULL;
    items.clear();

    delete hud; hud = NULL;
    delete map; map = NULL;
    bombs.clear();
    texProgram.free();
}

void Scene::resetPlayer() {
    player->setPosition(map->getSpawn());
    bombs.clear();
    for (Enemy* enemy : enemies) enemy->clearProjectiles();
}

void Scene::init() {
    initShaders();
    map = TileMap::createTileMap("levels/level01.txt", glm::vec2(0, 0), texProgram);

    // Textura y Sprites para los 3 estados de la Bomba
    bombTexture = new Texture();
    if (!bombTexture->loadFromFile("images/bomb.png", TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/bomb.png");
    bombTexture->setMinFilter(GL_NEAREST);
    bombTexture->setMagFilter(GL_NEAREST);

    const float tileSize = float(map->getTileSize());

    // Estado 1: Normal
    bombSprite = Sprite::createSprite(glm::vec2(tileSize, tileSize), glm::vec2(1.f, 1.f), bombTexture, &texProgram);
    bombSprite->setNumberAnimations(1);
    bombSprite->setAnimationSpeed(0, 1);
    bombSprite->addKeyframe(0, glm::vec2(0.f, 0.f));
    bombSprite->changeAnimation(0);

    // Estado 2: Medio
    mediumBombSprite = Sprite::createSprite(glm::vec2(tileSize * 1.125f, tileSize * 1.125f), glm::vec2(1.f, 1.f), bombTexture, &texProgram);
    mediumBombSprite->setNumberAnimations(1);
    mediumBombSprite->setAnimationSpeed(0, 1);
    mediumBombSprite->addKeyframe(0, glm::vec2(0.f, 0.f));
    mediumBombSprite->changeAnimation(0);

    // Estado 3: Grande
    largeBombSprite = Sprite::createSprite(glm::vec2(tileSize * 1.25f, tileSize * 1.25f), glm::vec2(1.f, 1.f), bombTexture, &texProgram);
    largeBombSprite->setNumberAnimations(1);
    largeBombSprite->setAnimationSpeed(0, 1);
    largeBombSprite->addKeyframe(0, glm::vec2(0.f, 0.f));
    largeBombSprite->changeAnimation(0);

    // Atlas de Fuego (9x7 casillas)
    explosionTexture = new Texture();
    if (!explosionTexture->loadFromFile("images/fire-atlas.png", TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/fire-atlas.png");
    explosionTexture->setMinFilter(GL_NEAREST);
    explosionTexture->setMagFilter(GL_NEAREST);

    const glm::vec2 fireCellSize(1.f / 9.f, 1.f / 7.f);

    // Mapeo corregido de columnas:
    // Center(6), Top(0), Bottom(1), Left(2), Right(3), VertMid(4), HorizMid(5)
    const int fireCols[BLAST_TYPE_COUNT] = { 6, 0, 1, 2, 3, 4, 5 };

    for (int i = 0; i < BLAST_TYPE_COUNT; ++i) {
        explosionSprites[i] = Sprite::createSprite(glm::vec2(tileSize, tileSize), fireCellSize, explosionTexture, &texProgram);
        explosionSprites[i]->setNumberAnimations(1);
        explosionSprites[i]->setAnimationSpeed(0, 10);

        for (int frame = 1; frame <= 5; ++frame) {
            explosionSprites[i]->addKeyframe(0, glm::vec2(fireCols[i] / 9.f, frame / 7.f));
        }
        explosionSprites[i]->changeAnimation(0);
    }

    // Texturas de enemigos.
    shooterTexture = new Texture();
    if (!shooterTexture->loadFromFile("images/shooter-atlas.png", TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/shooter-atlas.png");
    shooterTexture->setMinFilter(GL_NEAREST);
    shooterTexture->setMagFilter(GL_NEAREST);
    shooterBulletTexture = new Texture();
    if (!shooterBulletTexture->loadFromFile("images/shooter-bullet.png", TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/shooter-bullet.png");
    shooterBulletTexture->setMinFilter(GL_NEAREST);
    shooterBulletTexture->setMagFilter(GL_NEAREST);
    // Sprite provisional del walker: el antiguo personaje Bubble.
    walkerTexture = new Texture();
    if (!walkerTexture->loadFromFile("images/bub.png", TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/bub.png");
    walkerTexture->setMinFilter(GL_NEAREST);
    walkerTexture->setMagFilter(GL_NEAREST);

    spawnEnemies();

    // Objetos: atlas de 4 celdas de 32x32 en el orden de ItemType.
    itemTexture = new Texture();
    if (!itemTexture->loadFromFile("images/items-atlas.png", TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/items-atlas.png");
    itemTexture->setMinFilter(GL_NEAREST);
    itemTexture->setMagFilter(GL_NEAREST);
    itemSprite = Sprite::createSprite(glm::vec2(tileSize, tileSize),
        glm::vec2(1.f / ITEM_TYPE_COUNT, 1.f), itemTexture, &texProgram);
    itemSprite->setNumberAnimations(ITEM_TYPE_COUNT);
    for (int type = 0; type < ITEM_TYPE_COUNT; ++type) {
        itemSprite->setAnimationSpeed(type, 1);
        itemSprite->addKeyframe(type, glm::vec2(float(type) / ITEM_TYPE_COUNT, 0.f));
    }
    spawnItems();

    hud = new Hud();
    hud->init(texProgram);

    player = new Player();
    player->init(glm::ivec2(0, 0), texProgram);
    resetPlayer();
    player->setTileMap(map);

    projection = glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f);
    currentTime = 0.0f;
    update(0);
}

// Crea los enemigos del nivel (provisional: posiciones fijas hasta que el nivel las defina).
// Cada spawn: {columna, fila del suelo, mira a la izquierda}.
void Scene::spawnEnemies() {
    struct Spawn { int tileX, floorRow; bool facingLeft; };
    const float tileSize = float(map->getTileSize());

    // Shooters: pasillo inferior y repisa superior.
    const Spawn shooterSpawns[] = { { 14, 25, false }, { 24, 10, true } };
    for (const Spawn& spawn : shooterSpawns) {
        const glm::vec2 position(spawn.tileX * tileSize, spawn.floorRow * tileSize - Shooter::HEIGHT);
        Shooter* shooter = new Shooter(position, spawn.facingLeft);
        shooter->init(shooterTexture, shooterBulletTexture, texProgram);
        enemies.push_back(shooter);
    }

    // Walkers: suelo derecho y plataforma central (de esta se cae por el borde).
    const Spawn walkerSpawns[] = { { 42, 25, true }, { 16, 17, false } };
    for (const Spawn& spawn : walkerSpawns) {
        const glm::vec2 position(spawn.tileX * tileSize, spawn.floorRow * tileSize - Walker::HEIGHT);
        Walker* walker = new Walker(position, spawn.facingLeft);
        walker->init(walkerTexture, texProgram);
        enemies.push_back(walker);
    }

    for (Enemy* enemy : enemies) enemy->setTileMap(map);
}

// Objetos escondidos en arbustos del nivel 1 (provisional: posiciones fijas).
// Cada uno: {columna, fila, tipo, revelado}. La celda debe ser un arbusto.
void Scene::spawnItems() {
    if (map->hasItemSpawns()) {
        items.clear();
        for (const auto& spawn : map->getItemSpawns())
            items.push_back({spawn.x,spawn.y,static_cast<ItemType>(spawn.type),false});
        return;
    }
    items = {
        { 3, 24, ITEM_FIRE_UP, false },  // junto al inicio
        { 4, 22, ITEM_BOMB_UP, false },
        { 11, 20, ITEM_HEART, false },
        { 15, 16, ITEM_STAR, false },
    };
}

// Al tocar un objeto visible, el jugador lo recoge y se aplica su efecto.
void Scene::pickUpItems() {
    const movement::Body& body = player->getBody();
    const float tileSize = float(map->getTileSize());
    for (auto it = items.begin(); it != items.end();) {
        const float x = it->tileX * tileSize, y = it->tileY * tileSize;
        const bool touching = it->revealed && body.x < x + tileSize && body.x + body.width > x &&
            body.y < y + tileSize && body.y + body.height > y;
        if (!touching) {
            ++it;
            continue;
        }
        switch (it->type) {
        case ITEM_FIRE_UP: player->increaseFireRange(); break;
        case ITEM_BOMB_UP: player->increaseBombCapacity(); break;
        case ITEM_HEART:   player->gainLife(); break;
        case ITEM_STAR:    player->startStar(); break;
        default: break;
        }
        it = items.erase(it);
    }
}

// Quita una vida si el jugador puede recibir daño. Sin vidas, se reinicia el nivel.
void Scene::damagePlayer() {
    if (invulnerableMode || !player->canBeHurt()) return;
    player->loseLife();
    if (player->getLives() <= 0) restartLevel();
    else player->setPosition(map->getSpawn());
}

// Provisional hasta tener menú: al perder las 3 vidas se recarga el nivel
// (mapa, enemigos, tiempo) y el jugador vuelve a sus valores iniciales.
void Scene::restartLevel() {
    for (Enemy* enemy : enemies) delete enemy;
    enemies.clear();
    delete map;
    map = TileMap::createTileMap("levels/level01.txt", glm::vec2(0, 0), texProgram);
    player->setTileMap(map);
    player->resetStats();
    spawnEnemies();
    spawnItems();
    resetPlayer();
    timeLeftMs = LEVEL_TIME_SECONDS * 1000;
}

void Scene::update(int deltaTime) {
    currentTime += deltaTime;
    map->update(deltaTime);

    for (Bomb& bomb : bombs)
        if (bomb.update(deltaTime)) detonate(bomb);

    bombs.erase(std::remove_if(bombs.begin(), bombs.end(),
        [](const Bomb& bomb) { return bomb.isExpired(); }), bombs.end());

    if (player->update(deltaTime, bombs)) {
        const glm::vec2 pos = player->getPosition();
        const int tileSize = map->getTileSize();
        const int bombX = int(std::floor(pos.x / tileSize)) * tileSize;
        const int bombY = int(std::floor(pos.y / tileSize)) * tileSize;

        const bool alreadyPlaced = std::any_of(bombs.begin(), bombs.end(),
            [bombX, bombY](const Bomb& bomb) {
                return !bomb.isExploding() && bomb.getX() == bombX && bomb.getY() == bombY;
            });

        const std::size_t activeBombCount = std::count_if(bombs.begin(), bombs.end(),
            [](const Bomb& bomb) { return !bomb.isExploding(); });

        if (!map->isSolid(bombX / tileSize, bombY / tileSize) && !alreadyPlaced && player->canPlaceBomb(activeBombCount)) {
            if (activeBombCount == 0) {
                for (int i = 0; i < BLAST_TYPE_COUNT; ++i) {
                    explosionSprites[i]->changeAnimation(0);
                }
            }
            bombs.emplace_back(bombX, bombY, tileSize, Bomb::DEFAULT_FUSE_MS, player->getFireRange());
        }
    }

    killEnemiesInBlast();
    pickUpItems();

    // Daño al jugador: bala, contacto con un enemigo o fuego de una explosión.
    // Primero se recoge si hay daño y después se aplica, porque damagePlayer()
    // puede reiniciar el nivel y borrar los enemigos.
    const movement::Body& playerBody = player->getBody();
    bool playerHurt = false;
    for (Enemy* enemy : enemies) {
        enemy->update(deltaTime, playerBody);
        if (enemy->hitsPlayer(playerBody) || enemy->touches(playerBody)) playerHurt = true;
    }
    if (touchesFire(playerBody)) playerHurt = true;
    if (playerHurt) damagePlayer();

    // Tiempo límite: al llegar a 0 se pierde una vida y el reloj vuelve a empezar.
    timeLeftMs -= deltaTime;
    if (timeLeftMs <= 0) {
        timeLeftMs = LEVEL_TIME_SECONDS * 1000;
        damagePlayer();
    }

    const bool hasExplodingBomb = std::any_of(bombs.begin(), bombs.end(),
        [](const Bomb& bomb) { return bomb.isExploding(); });

    if (hasExplodingBomb) {
        for (int i = 0; i < BLAST_TYPE_COUNT; ++i) {
            explosionSprites[i]->update(deltaTime);
        }
    }

    const glm::vec2 pos = player->getPosition();
    const glm::ivec2 size = map->getPixelSize();
    auto camera = view::follow(pos.x,pos.y,size.x,size.y,map->getTileSize());
    if (mapEffectsTest) camera = view::follow(28.f*32,15.f*32,size.x,size.y,map->getTileSize());
    projection = glm::ortho(camera.x,camera.x+camera.width,camera.y+camera.height,camera.y);
}

void Scene::detonate(Bomb& bomb) {
    const int tileSize = map->getTileSize();
    const int centerX = bomb.getX() / tileSize;
    const int centerY = bomb.getY() / tileSize;

    std::vector<Bomb::BlastCell> blastCells;
    std::vector<glm::ivec2> affectedCells;

    blastCells.push_back({ centerX, centerY, BLAST_CENTER });
    affectedCells.push_back(glm::ivec2(centerX, centerY));

    const int directions[4][2] = {
        { 1, 0 },  // Derecha
        { -1, 0 }, // Izquierda
        { 0, 1 },  // Abajo
        { 0, -1 }  // Arriba
    };

    for (const auto& direction : directions) {
        int dx = direction[0];
        int dy = direction[1];

        const int maxSteps = Bomb::blastDistance(centerX,centerY,dx,dy,bomb.getFireRange(),
            [this](int x,int y) { return map->isSolid(x,y); },
            [this](int x,int y) { return map->isFoliage(x,y) || map->isWaterfallHead(x,y); });

        for (int step = 1; step <= maxSteps; ++step) {
            int x = centerX + dx * step;
            int y = centerY + dy * step;

            bool isEnd = (step == maxSteps);
            int cellType = BLAST_CENTER;

            if (dx > 0) {
                cellType = isEnd ? BLAST_RIGHT_END : BLAST_HORIZONTAL_MID;
            }
            else if (dx < 0) {
                cellType = isEnd ? BLAST_LEFT_END : BLAST_HORIZONTAL_MID;
            }
            else if (dy < 0) {
                cellType = isEnd ? BLAST_TOP_END : BLAST_VERTICAL_MID;
            }
            else if (dy > 0) {
                cellType = isEnd ? BLAST_BOTTOM_END : BLAST_VERTICAL_MID;
            }

            blastCells.push_back({ x, y, cellType });
            affectedCells.push_back(glm::ivec2(x, y));
        }
    }

    // Los objetos escondidos en arbustos alcanzados por el fuego se hacen visibles.
    for (Item& item : items) {
        if (item.revealed || !map->isFoliage(item.tileX, item.tileY)) continue;
        for (const glm::ivec2& cell : affectedCells)
            if (cell.x == item.tileX && cell.y == item.tileY) item.revealed = true;
    }
    map->hitWaterfallHeads(affectedCells);
    map->destroyFoliage(affectedCells);
    bomb.setBlastCells(blastCells);
}

// ¿Toca el cuerpo alguna celda de una explosión activa (dura 0.5 s)?
bool Scene::touchesFire(const movement::Body& body) const {
    const float tileSize = float(map->getTileSize());
    for (const Bomb& bomb : bombs) {
        if (!bomb.isExploding()) continue;
        for (const Bomb::BlastCell& cell : bomb.getBlastCells()) {
            const float cellX = cell.x * tileSize, cellY = cell.y * tileSize;
            if (body.x < cellX + tileSize && body.x + body.width > cellX &&
                body.y < cellY + tileSize && body.y + body.height > cellY)
                return true;
        }
    }
    return false;
}

// Cualquier enemigo que toque el fuego muere: se borra (con sus balas, si tiene).
void Scene::killEnemiesInBlast() {
    for (auto it = enemies.begin(); it != enemies.end();) {
        if (touchesFire((*it)->getBody())) {
            delete *it;
            it = enemies.erase(it);
        } else {
            ++it;
        }
    }
}

void Scene::render() {
    glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT-Hud::HEIGHT);
    glm::mat4 modelview;

    texProgram.use();
    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    modelview = glm::mat4(1.0f);
    texProgram.setUniformMatrix4f("modelview", modelview);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
    map->render();

    for (const Item& item : items) {
        if (!item.revealed) continue;
        itemSprite->changeAnimation(item.type);
        itemSprite->setPosition(glm::vec2(float(item.tileX * map->getTileSize()),
            float(item.tileY * map->getTileSize())));
        itemSprite->render();
    }

    for (const Bomb& bomb : bombs) {
        if (bomb.isExploding()) {
            for (const Bomb::BlastCell& cell : bomb.getBlastCells()) {
                int type = cell.type;
                if (type >= 0 && type < BLAST_TYPE_COUNT && explosionSprites[type] != NULL) {
                    explosionSprites[type]->setPosition(glm::vec2(
                        float(cell.x * map->getTileSize()),
                        float(cell.y * map->getTileSize())
                    ));
                    explosionSprites[type]->render();
                }
            }
        }
        else {
            const glm::vec2 bombPosition(float(bomb.getX()), float(bomb.getY()));
            int ms = bomb.getRemainingMs();

            if (ms <= 500) {
                const float offset = map->getTileSize() * 0.125f;
                largeBombSprite->setPosition(bombPosition - glm::vec2(offset, offset));
                largeBombSprite->render();
            }
            else if (ms <= 1500) {
                const float offset = map->getTileSize() * 0.0625f;
                mediumBombSprite->setPosition(bombPosition - glm::vec2(offset, offset));
                mediumBombSprite->render();
            }
            else {
                bombSprite->setPosition(bombPosition);
                bombSprite->render();
            }
        }
    }
    for (Enemy* enemy : enemies) enemy->render();
    player->render();

    glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    // HUD en su propia franja, fuera de las nueve filas de juego.
    texProgram.setUniformMatrix4f("projection",
        glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f));
    HudInfo info;
    info.lives = player->getLives();
    info.maxLives = Player::MAX_LIVES;
    info.secondsLeft = (timeLeftMs + 999) / 1000;
    info.enemiesLeft = int(enemies.size());
    info.level = level;
    info.fireRange = player->getFireRange();
    info.bombCapacity = int(player->getBombCapacity());
    info.invulnerable = invulnerableMode;
    info.starSeconds = (player->getStarMs() + 999) / 1000;
    hud->render(info);
}

void Scene::initShaders() {
    Shader vShader, fShader;

    vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
    if (!vShader.isCompiled()) {
        std::cout << "Vertex Shader Error\n" << vShader.log() << std::endl;
    }
    fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
    if (!fShader.isCompiled()) {
        std::cout << "Fragment Shader Error\n" << fShader.log() << std::endl;
    }
    texProgram.init();
    texProgram.addShader(vShader);
    texProgram.addShader(fShader);
    texProgram.bindFragmentOutput("outColor");
    texProgram.link();
    if (!texProgram.isLinked()) {
        std::cout << "Shader Linking Error\n" << texProgram.log() << std::endl;
        throw std::runtime_error("Shader link failed");
    }
    vShader.free();
    fShader.free();
}
// Deterministic hidden rendering test, requested through --map-effects-test only.
void Scene::startMapEffectsTest() {
    mapEffectsTest = true;
    invulnerableMode = true;
    player->setPosition(glm::vec2(28.f*32,12.f*32));
    bombs.emplace_back(28*32,12*32,32,0,1);
    bombs.emplace_back(5*32,22*32,32,0,1);
}
