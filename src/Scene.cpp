#include <iostream>
#include <fstream>
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
    explosionTexture = NULL;
    for (int i = 0; i < BLAST_TYPE_COUNT; ++i) {
        explosionSprites[i] = NULL;
    }
    shooterTexture = NULL;
    shooterBulletTexture = NULL;
    walkerTexture = NULL;
    booTexture = NULL;
    doorTexture = NULL;
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
    delete booTexture; booTexture = NULL;
    delete doorTexture; doorTexture = NULL;

    delete itemSprite; itemSprite = NULL;
    delete itemTexture; itemTexture = NULL;
    items.clear();

    delete hud; hud = NULL;
    delete map; map = NULL;
    bombs.clear();
    texProgram.free();
}

void Scene::resetPlayer() {
    if (player->isDying()) return;  // la tecla R no interrumpe la animación de muerte
    player->setPosition(map->getSpawn());
    bombs.clear();
    for (Enemy* enemy : enemies) enemy->clearProjectiles();
}

void Scene::init() {
    initShaders();
    map = TileMap::createTileMap(levelFile(level), glm::vec2(0, 0), texProgram);

    bombTexture = new Texture();
    if (!bombTexture->loadFromFile("images/original-bomb-atlas.png",TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load original bomb atlas");
    bombTexture->setMinFilter(GL_NEAREST);
    bombTexture->setMagFilter(GL_NEAREST);
    const float tileSize=float(map->getTileSize());
    const float sourcePixel=tileSize/16.f;
    bombSprite=Sprite::createSprite(glm::vec2(20.f*sourcePixel),glm::vec2(1.f/3.f,1.f),bombTexture,&texProgram);
    bombSprite->setNumberAnimations(3);
    for(int frame=0;frame<3;++frame) {
        bombSprite->setAnimationSpeed(frame,1);
        bombSprite->addKeyframe(frame,glm::vec2(frame/3.f,0.f));
    }
    bombSprite->changeAnimation(2);

    explosionTexture = new Texture();
    if (!explosionTexture->loadFromFile("images/original-fire-atlas.png",TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load original fire atlas");
    explosionTexture->setMinFilter(GL_NEAREST);
    explosionTexture->setMagFilter(GL_NEAREST);
    for(int type=0;type<BLAST_TYPE_COUNT;++type) {
        explosionSprites[type]=Sprite::createSprite(glm::vec2(tileSize),glm::vec2(1.f/7.f,1.f/3.f),explosionTexture,&texProgram);
        explosionSprites[type]->setNumberAnimations(3);
        for(int frame=0;frame<3;++frame) {
            explosionSprites[type]->setAnimationSpeed(frame,1);
            explosionSprites[type]->addKeyframe(frame,glm::vec2(type/7.f,frame/3.f));
        }
        explosionSprites[type]->changeAnimation(0);
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
    // Walker: Koopa Troopa, atlas generado por tools/build_koopa.py.
    walkerTexture = new Texture();
    if (!walkerTexture->loadFromFile("images/koopa-atlas.png", TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/koopa-atlas.png");
    walkerTexture->setMinFilter(GL_NEAREST);
    walkerTexture->setMagFilter(GL_NEAREST);
    // Boo: atlas generado por tools/build_boo.py.
    booTexture = new Texture();
    if (!booTexture->loadFromFile("images/boo-atlas.png", TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/boo-atlas.png");
    booTexture->setMinFilter(GL_NEAREST);
    booTexture->setMagFilter(GL_NEAREST);

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

    // Puerta-portal: atlas generado por tools/build_portal.py.
    doorTexture = new Texture();
    if (!doorTexture->loadFromFile("images/portal-atlas.png", TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load images/portal-atlas.png");
    doorTexture->setMinFilter(GL_NEAREST);
    doorTexture->setMagFilter(GL_NEAREST);
    setupDoor();

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

    // Boos: flotan en zonas abiertas. Cada spawn: {columna, fila de la hitbox}.
    const Spawn booSpawns[] = { { 20, 15, true }, { 38, 5, true } };
    for (const Spawn& spawn : booSpawns) {
        Boo* boo = new Boo(glm::vec2(spawn.tileX * tileSize, spawn.floorRow * tileSize), spawn.facingLeft);
        boo->init(booTexture, texProgram);
        enemies.push_back(boo);
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
        case ITEM_HEART:   player->collectGoldHeart(); break;
        case ITEM_STAR:    player->startStar(); break;
        default: break;
        }
        it = items.erase(it);
    }
}

// Quita una vida si el jugador puede recibir daño y empieza su animación de
// muerte. Al terminarla, update() lo hace reaparecer o acaba la partida.
void Scene::damagePlayer() {
    if (invulnerableMode || !player->canBeHurt()) return;
    if (player->takeHit() != PlayerHealth::Damage::LifeLost) return;
    player->startDying();
}

// Enemigos que siguen vivos (los que reproducen su animación de muerte no cuentan).
int Scene::aliveEnemies() const {
    int alive = 0;
    for (const Enemy* enemy : enemies)
        if (!enemy->isDying()) ++alive;
    return alive;
}

// levels/level01.txt, levels/level02.txt...
std::string Scene::levelFile(int number) {
    return std::string("levels/level") + (number < 10 ? "0" : "") + std::to_string(number) + ".txt";
}

bool Scene::levelExists(int number) {
    return number >= 1 && number <= LEVEL_COUNT && std::ifstream(levelFile(number)).good();
}

void Scene::startNewGame(int number) {
    player->resetStats();
    startLevel(number);
}

void Scene::startLevel(int number) {
    level = number;
    loadLevel();
}

// Carga (o recarga) el nivel actual: mapa, enemigos, objetos, puerta y tiempo.
// Los enemigos aún tienen posiciones fijas del nivel 1 (spawnEnemies).
void Scene::loadLevel() {
    for (Enemy* enemy : enemies) delete enemy;
    enemies.clear();
    delete map;
    map = TileMap::createTileMap(levelFile(level), glm::vec2(0, 0), texProgram);
    result = RUNNING;
    timeUp = false;
    player->setTileMap(map);
    spawnEnemies();
    spawnItems();
    setupDoor();
    resetPlayer();
    timeLeftMs = LEVEL_TIME_SECONDS * 1000;
}

// Coloca la puerta cerrada sobre el marcador del mapa. El marcador es la casilla
// inferior izquierda de un hueco de 2x2 tiles; la puerta se apoya en su suelo.
void Scene::setupDoor() {
    levelHasDoor = map->hasDoor();
    if (!levelHasDoor) return;
    const float tileSize = float(map->getTileSize());
    const glm::ivec2 tile = map->getDoorTile();
    door.init(doorTexture, texProgram, glm::vec2((tile.x + 1) * tileSize, (tile.y + 1) * tileSize));
}

// Tecla K: mata a todos los enemigos (y sus balas). La puerta se abrirá sola.
void Scene::killAllEnemies() {
    for (Enemy* enemy : enemies) enemy->kill();
}

void Scene::update(int deltaTime) {
    if (result != RUNNING) return;  // Game ya está cambiando de pantalla

    // Muerte del jugador: el resto del mundo se congela mientras dura la animación.
    // Al acabar, reaparece en el inicio o, sin vidas o sin tiempo, se pierde.
    if (player->isDying()) {
        player->updateDeath(deltaTime);
        if (player->deathFinished()) {
            if (timeUp || player->getLives() <= 0) result = GAME_OVER;
            else player->setPosition(map->getSpawn());
        }
        return;
    }

    currentTime += deltaTime;
    map->update(deltaTime);

    for (Bomb& bomb : bombs)
        if (bomb.update(deltaTime)) detonate(bomb);

    bombs.erase(std::remove_if(bombs.begin(), bombs.end(),
        [](const Bomb& bomb) { return bomb.isExpired(); }), bombs.end());

    if (player->update(deltaTime, bombs)) placeBomb();

    killEnemiesInBlast();
    pickUpItems();

    // Daño al jugador: bala, contacto con un enemigo o fuego de una explosión.
    // Primero se recoge si hay daño y después se aplica, porque damagePlayer()
    // puede reiniciar el nivel y borrar los enemigos.
    const movement::Body& playerBody = player->getBody();
    bool playerHurt = false;
    for (Enemy* enemy : enemies) {
        // Los que están muriendo solo avanzan su animación y ya no hacen daño.
        if (enemy->isDying()) {
            enemy->updateDeath(deltaTime);
            continue;
        }
        enemy->update(deltaTime, playerBody, player->isFacingLeft());
        if (enemy->hitsPlayer(playerBody) || enemy->touches(playerBody)) playerHurt = true;
    }
    if (touchesFire(playerBody)) playerHurt = true;
    if (playerHurt) damagePlayer();

    // Se borran los enemigos cuya animación de muerte ha terminado.
    for (auto it = enemies.begin(); it != enemies.end();) {
        if ((*it)->isDead()) {
            delete *it;
            it = enemies.erase(it);
        } else {
            ++it;
        }
    }
    if (player->isDying()) return;  // a partir del siguiente frame, animación de muerte

    // Puerta: se abre al no quedar enemigos vivos; abierta, el jugador completa el nivel.
    if (levelHasDoor) {
        if (aliveEnemies() == 0) door.open();
        door.update(deltaTime);
        if (door.isOpen() && door.contains(player->getBody())) result = LEVEL_COMPLETED;
    }

    // Tiempo límite: al llegar a 0 la partida se pierde directamente, sin importar
    // las vidas, el corazón dorado o la estrella (tras la animación de muerte).
    // Solo la tecla G (modo de prueba) lo evita: entonces el reloj vuelve a empezar.
    timeLeftMs -= deltaTime;
    if (timeLeftMs <= 0) {
        if (invulnerableMode) timeLeftMs = LEVEL_TIME_SECONDS * 1000;
        else {
            timeLeftMs = 0;
            timeUp = true;
            player->startDying();
        }
    }

    const glm::vec2 pos = player->getPosition();
    const glm::ivec2 size = map->getPixelSize();
    auto camera = view::follow(pos.x,pos.y,size.x,size.y,map->getTileSize());
    if (originalEffectsTest) camera = view::follow(20.f*32,7.f*32,size.x,size.y,map->getTileSize());
    else if (mapEffectsTest) camera = view::follow(28.f*32,15.f*32,size.x,size.y,map->getTileSize());
    projection = glm::ortho(camera.x,camera.x+camera.width,camera.y+camera.height,camera.y);
}

void Scene::placeBomb() {
    const movement::Body& body = player->getBody();
    const int tileSize = map->getTileSize();
    const int bombX = Bomb::snapToBodyCell(body.x,body.width,tileSize);
    const int bombY = Bomb::snapToBodyCell(body.y,body.height,tileSize);

    const bool alreadyPlaced = std::any_of(bombs.begin(), bombs.end(),
        [bombX, bombY](const Bomb& bomb) {
            return !bomb.isExploding() && bomb.getX() == bombX && bomb.getY() == bombY;
        });

    const std::size_t activeBombCount = std::count_if(bombs.begin(), bombs.end(),
        [](const Bomb& bomb) { return !bomb.isExploding(); });

    if (!map->isSolid(bombX / tileSize, bombY / tileSize) && !alreadyPlaced && player->canPlaceBomb(activeBombCount)) {
        bombs.emplace_back(bombX, bombY, tileSize, Bomb::DEFAULT_FUSE_MS, player->getFireRange());
        bombs.back().allowOwnerExit(body.x,body.y,body.width,body.height);
    }
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
// El fuego mata a los enemigos que toca: empiezan su animación de muerte.
void Scene::killEnemiesInBlast() {
    for (Enemy* enemy : enemies)
        if (!enemy->isDying() && touchesFire(enemy->getBody())) enemy->kill();
}

void Scene::render() {
    glViewport(0,Hud::HEIGHT,SCREEN_WIDTH,SCREEN_HEIGHT-Hud::HEIGHT);
    glm::mat4 modelview;

    texProgram.use();
    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    modelview = glm::mat4(1.0f);
    texProgram.setUniformMatrix4f("modelview", modelview);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
    map->render();
    if (levelHasDoor) door.render();

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
                    explosionSprites[type]->changeAnimation(bomb.getExplosionFrame());
                    explosionSprites[type]->setPosition(glm::vec2(
                        float(cell.x * map->getTileSize()),
                        float(cell.y * map->getTileSize())
                    ));
                    explosionSprites[type]->render();
                }
            }
        }
        else {
            const float pixel=map->getTileSize()/16.f;
            bombSprite->changeAnimation(bomb.getBombFrame());
            bombSprite->setPosition(glm::vec2(bomb.getX()-2.f*pixel,bomb.getY()-3.f*pixel));
            bombSprite->render();
        }
    }
    for (Enemy* enemy : enemies) enemy->render();
    player->render();

    glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    // HUD en su propia franja, fuera de las nueve filas de juego.
    texProgram.setUniformMatrix4f("projection",
        glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f));
    HudInfo info;
    info.spareLives = player->getSpareLives();
    info.goldHeart = player->hasGoldHeart();
    info.secondsLeft = (timeLeftMs + 999) / 1000;
    info.enemiesLeft = aliveEnemies();
    info.fireRange = player->getFireRange();
    info.bombCapacity = int(player->getBombCapacity());
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

// Integration test reached only through the hidden --hud-test command line mode.
void Scene::runHudTestStep(int step) {
    if (step==0) {
        player->resetStats();
        invulnerableMode = false;
        timeLeftMs = LEVEL_TIME_SECONDS*1000;
        player->setPosition(map->getSpawn()+glm::vec2(64.f,0.f));
        if (player->getSpareLives()!=2) throw std::runtime_error("HUD initial life count failed");
    } else if (step==1) {
        const glm::vec2 pos=player->getPosition();
        items.push_back({int(pos.x)/map->getTileSize(),int(pos.y)/map->getTileSize(),ITEM_HEART,true});
        pickUpItems();
        if (!player->hasGoldHeart() || player->getLives()!=3)
            throw std::runtime_error("Gold heart pickup added lives or failed to grant protection");
    } else if (step==2) {
        const glm::vec2 before=player->getPosition();
        damagePlayer();
        const glm::vec2 after=player->getPosition();
        if (player->hasGoldHeart() || player->getLives()!=3 || before.x!=after.x || before.y!=after.y || player->canBeHurt())
            throw std::runtime_error("Gold heart hit must keep the player in place and preserve all lives");
    } else if (step==3) {
        player->update(100,bombs); // Capture the invisible half of the blink cycle.
    } else if (step==4) {
        player->update(1900,bombs);
        damagePlayer();
        if (player->getSpareLives()!=1) throw std::runtime_error("First life loss must display 1");
    } else if (step==5) {
        player->update(2000,bombs);
        damagePlayer();
        if (player->getSpareLives()!=0 || player->getLives()!=1)
            throw std::runtime_error("Last live attempt must display 0");
    }
}

// Hidden integration check: uses the real Player update and Scene placement logic.
void Scene::runBombCollisionTest() {
    auto require=[](bool ok,const char* message) { if(!ok) throw std::runtime_error(message); };
    auto& game=Game::instance();
    auto move=[&](int direction,int frames,bool jump=false) {
        game.keyReleased(GLFW_KEY_LEFT); game.keyReleased(GLFW_KEY_RIGHT); game.keyReleased(GLFW_KEY_UP);
        if(direction) game.keyPressed(direction>0 ? GLFW_KEY_RIGHT : GLFW_KEY_LEFT);
        if(jump) game.keyPressed(GLFW_KEY_UP);
        for(int i=0;i<frames;++i) { player->update(8,bombs); if(i==0) game.keyReleased(GLFW_KEY_UP); }
        game.keyReleased(GLFW_KEY_LEFT); game.keyReleased(GLFW_KEY_RIGHT);
    };
    const int tile=map->getTileSize(), x=20*tile, floorY=24*tile;
    for(float offset : {-4.f,4.f,15.f}) {
        bombs.clear();
        player->setPosition(glm::vec2(x+offset,float(floorY)));
        placeBomb();
        require(bombs.size()==1 && bombs[0].getX()==x && bombs[0].getY()==floorY,"Bomb drifted to the adjacent tile");
    }
    bombs.clear(); bombs.emplace_back(x,floorY,tile);
    player->setPosition(glm::vec2(float(x-tile),float(floorY)));
    move(1,50);
    require(player->getPosition().x==x-tile,"Player crossed the left side of a bomb");
    player->setPosition(glm::vec2(float(x+tile),float(floorY)));
    move(-1,50);
    require(player->getPosition().x==x+tile,"Player crossed the right side of a bomb");

    bombs.clear(); bombs.emplace_back(x,22*tile,tile);
    player->setPosition(glm::vec2(float(x),float(floorY)));
    move(0,1);
    game.keyPressed(GLFW_KEY_UP);
    bool bumped=false;
    for(int i=0;i<100;++i) {
        player->update(8,bombs);
        game.keyReleased(GLFW_KEY_UP);
        require(player->getBody().y>=23*tile,"Player jumped through a bomb overhead");
        if(player->getBody().y==23*tile && player->getBody().vy==0) bumped=true;
    }
    require(bumped,"Bomb underside was not reached in the integration test");

    bombs.clear(); bombs.emplace_back(x,23*tile,tile);
    player->setPosition(glm::vec2(float(x),20.f*tile));
    move(0,200);
    require(player->getBody().grounded && player->getBody().y==22*tile,"Player did not land on a floating bomb");
    move(0,200,true);
    require(player->getBody().grounded && player->getBody().y==22*tile,"Player cannot jump from and land back on a bomb");

    bombs.clear(); player->setPosition(glm::vec2(float(x),float(floorY)));
    move(0,1); placeBomb();
    move(1,70);
    require(player->getPosition().x>=x+tile && !bombs[0].canOwnerExit(),"New bomb trapped the player or failed to become solid");
    move(-1,100);
    require(player->getPosition().x==x+tile,"Owner could re-enter a bomb after leaving");
    resetPlayer();
}

// Hidden render test: three fuse sizes, growing/shrinking fire, original item icons.
void Scene::startOriginalEffectsTest() {
    originalEffectsTest=true;
    invulnerableMode=true;
    bombs.clear();
    bombs.emplace_back(20*32,7*32,32,Bomb::DEFAULT_FUSE_MS,2);
    for(int type=0;type<ITEM_TYPE_COUNT;++type) items.push_back({17+type,4,static_cast<ItemType>(type),true});
}
