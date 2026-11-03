#pragma once

// Objetos escondidos en los arbustos (tile de follaje). Aparecen cuando una
// explosión destruye su arbusto y se recogen al tocarlos.
// El orden coincide con las celdas de items-atlas.png (tools/build_items.py).
enum ItemType {
    ITEM_FIRE_UP,  // +1 alcance de la explosión
    ITEM_BOMB_UP,  // +1 bomba simultánea
    ITEM_HEART,    // recupera una vida (máximo Player::MAX_LIVES)
    ITEM_STAR,     // invulnerable durante Player::STAR_MS
    ITEM_TYPE_COUNT
};

struct Item {
    int tileX, tileY;  // celda del arbusto que lo esconde
    ItemType type;
    bool revealed;     // false mientras el arbusto sigue en pie
};
