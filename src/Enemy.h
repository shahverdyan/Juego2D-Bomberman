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
    virtual void update(int deltaTime, const movement::Body& player) = 0;
    virtual void render();

    // Proyectiles. Por defecto un enemigo no dispara: los que sí (Shooter)
    // sobrescriben estos métodos.
    virtual bool hitsPlayer(const movement::Body&) { return false; }
    virtual void clearProjectiles() {}

    const movement::Body& getBody() const { return body; }
    // Contacto cuerpo a cuerpo con otro cuerpo (el jugador).
    bool touches(const movement::Body& other) const;

protected:
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
};
