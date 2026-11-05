#pragma once
#include <vector>
#include "Enemy.h"

// Jefe final (el demonio del mundo Evil de Pocket Bomberman). Repite un ciclo:
//   1. Flota despacio por toda la sala hacia puntos al azar (también bajos:
//      es cuando se le alcanza con las bombas).
//   2. Una vez sí y otra no, se teletransporta: desaparece y reaparece en otro
//      sitio, lejos del jugador, para sorprenderle.
//   3. Se para con las alas levantadas y ataca. Los ataques van por turnos:
//      - dos bolas grandes que rebotan por la sala durante unos segundos;
//      - cuatro bolas en diagonal (en forma de X);
//      - una estatua que cae desde el techo encima del jugador.
// Aguanta MAX_HEALTH explosiones; tras cada una es invulnerable HURT_MS
// (parpadea), para que una misma explosión no le quite varias vidas.
// Atraviesa el terreno y hace daño por contacto.
class Boss : public Enemy {
public:
    static constexpr float WIDTH = 52.f, HEIGHT = 52.f;  // hitbox; el dibujo mide 64x64
    static const int MAX_HEALTH = 5;
    static const int HURT_MS = 1500;
    static constexpr float FLOAT_SPEED = 45.f;      // unidades/s (el jugador anda a 160)
    static constexpr float FIREBALL_SPEED = 140.f;  // bolas en diagonal
    static constexpr float BOUNCE_SPEED = 110.f;    // bolas grandes que rebotan

    // position: esquina superior izquierda de la hitbox.
    // minX/maxX y minY/maxY: límites de la hitbox dentro de la sala.
    Boss(const glm::vec2& position, float minX, float maxX, float minY, float maxY);
    ~Boss() override;

    // Las texturas pertenecen a la Scene.
    void init(Texture* bossTexture, Texture* fireballTexture, Texture* statueTexture,
              ShaderProgram& program);
    void update(int deltaTime, const movement::Body& player, bool playerFacingLeft) override;
    void render() override;

    bool touches(const movement::Body& other) const override;  // no, si está oculto
    bool hitsPlayer(const movement::Body& player) override;    // bolas de fuego
    void clearProjectiles() override;
    void hitByExplosion() override;
    void takeSpawnedEnemies(std::vector<Enemy*>& out) override;

protected:
    int deathAnimation() const override;
    void moveWhileDying(float seconds) override;  // se hunde despacio mientras parpadea
    glm::vec2 spriteOffset() const override;

private:
    enum Phase { FLOAT, VANISH, APPEAR, ATTACK };
    enum Attack { BOUNCING_BALLS, DIAGONAL_SHOTS, FALLING_STATUES, ATTACK_COUNT };
    // Bola de fuego: esquina superior izquierda del dibujo, velocidad y tamaño.
    // burstMs < 0 mientras vuela; desde 0 cuenta el tiempo del estallido.
    // Las grandes (big) rebotan en el terreno y estallan al acabar su lifeMs.
    struct Fireball { float x, y, vx, vy; int burstMs; bool big; int lifeMs; };

    void startPhase(Phase newPhase);
    void attack(const movement::Body& player);
    void updateFireballs(int deltaTime);
    glm::vec2 randomPosition() const;  // punto al azar para la hitbox dentro de los límites
    bool hidden() const;               // invisible a mitad del teletransporte

    float minX, maxX, minY, maxY;
    Phase phase = FLOAT;
    int phaseMs = 0;
    int cycle = 0;                  // ciclos de flotar; los impares acaban en teletransporte
    Attack nextAttack = BOUNCING_BALLS;
    bool attacked = false;          // ya ha lanzado el ataque de esta fase ATTACK
    glm::vec2 target;               // hacia dónde flota
    int health = MAX_HEALTH;
    int hurtMs = 0;
    int timeMs = 0;                 // para el balanceo y la animación de las bolas

    std::vector<Fireball> fireballs;
    std::vector<Enemy*> newStatues;  // creadas y aún no entregadas a la Scene
    Sprite* fireballSprite = nullptr;
    Sprite* bigFireballSprite = nullptr;
    Texture* statueTexture = nullptr;
    ShaderProgram* program = nullptr;
};
