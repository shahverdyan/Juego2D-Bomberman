#pragma once
#include "Sprite.h"
#include "TileMap.h"
#include "Movement.h"

// Clase base de todos los enemigos. Guarda lo común (cuerpo físico, dirección,
// sprite y mapa) y ofrece utilidades de movimiento. Cada tipo de enemigo
// hereda de ella e implementa su comportamiento en update().
//
// La Scene trata a todos por igual mediante punteros Enemy*: los actualiza,
// los dibuja, comprueba el contacto con el jugador y los mata con el fuego.
class Enemy {
public:
    // position: esquina superior izquierda del cuerpo.
    Enemy(const glm::vec2& position, float width, float height, bool facingLeft);
    virtual ~Enemy();
    Enemy(const Enemy&) = delete;
    Enemy& operator=(const Enemy&) = delete;

    void setTileMap(TileMap* tileMap) { map = tileMap; }
    // playerFacingLeft: hacia dónde mira el jugador (lo usa el Boo).
    virtual void update(int deltaTime, const movement::Body& player, bool playerFacingLeft) = 0;
    virtual void render();

    // Proyectiles. Por defecto un enemigo no dispara: los que sí (Shooter)
    // sobrescriben estos métodos.
    virtual bool hitsPlayer(const movement::Body&) { return false; }
    virtual void clearProjectiles() {}

    const movement::Body& getBody() const { return body; }
    // Contacto cuerpo a cuerpo con otro cuerpo (el jugador).
    bool touches(const movement::Body& other) const;

    // Muerte (fuego o tecla K): deja de hacer daño, reproduce su animación
    // durante DEATH_MS con updateDeath() y después la Scene lo borra.
    static const int DEATH_MS = 1000;
    void kill();
    void updateDeath(int deltaTime);
    bool isDying() const { return dying; }
    bool isDead() const { return dying && deathMs >= DEATH_MS; }

protected:
    // Animación de muerte de cada tipo (índice de animación de su sprite).
    virtual int deathAnimation() const = 0;
    // Movimiento mientras muere. Por defecto, como en Mario: un salto hacia
    // arriba y cae atravesando el suelo.
    virtual void moveWhileDying(float seconds);
    // Desplazamiento del dibujo respecto a la hitbox (el Boo lo centra).
    virtual glm::vec2 spriteOffset() const { return glm::vec2(0.f, 0.f); }

    // Gravedad y colisión vertical con la misma física que el jugador (sin salto).
    void applyGravity(float seconds);
    // Avanza hacia donde mira. Devuelve true si una pared lo ha frenado.
    bool walk(float speed, float seconds);
    // ¿Hay suelo justo delante de los pies?
    bool floorAhead() const;
    void updateSprite(int deltaTime, int anim);

    movement::Body body;
    bool facingLeft;
    Sprite* sprite = nullptr;
    TileMap* map = nullptr;
    bool dying = false;
    int deathMs = 0;
};
