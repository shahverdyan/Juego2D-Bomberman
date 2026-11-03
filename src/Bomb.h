#pragma once
#include <algorithm>
#include <vector>

class Bomb {
public:
    // Estructura actualizada para incluir el tipo/pieza de animación de fuego
    struct BlastCell { 
        int x, y; 
        int type; // Índice del tipo de pieza (centro, medio horizontal/vertical, puntas)
    };

    enum { 
        DEFAULT_FUSE_MS = 3000, 
        DEFAULT_FIRE_RANGE = 1,
        DEFAULT_EXPLOSION_MS = 500 
    };

    Bomb(int x, int y, int tileSize, int fuseDurationMs = DEFAULT_FUSE_MS,
        int fireRange = DEFAULT_FIRE_RANGE)
        : x(x), y(y), tileSize(tileSize), remainingMs(std::max(0, fuseDurationMs)),
          fireRange(std::max(1, fireRange)), exploding(false), explosionRemainingMs(0) {}

    bool update(int deltaTime) {
        const int elapsed = std::max(0, deltaTime);
        if (exploding) {
            explosionRemainingMs = std::max(0, explosionRemainingMs - elapsed);
            return false;
        }
        remainingMs = std::max(0, remainingMs - elapsed);
        if (remainingMs != 0) return false;
        exploding = true;
        explosionRemainingMs = DEFAULT_EXPLOSION_MS;
        return true;
    }

    void setBlastCells(const std::vector<BlastCell>& cells) { blastCells = cells; }
    bool isExploding() const { return exploding; }
    bool isExpired() const { return exploding && explosionRemainingMs == 0; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getTileSize() const { return tileSize; }
    int getFireRange() const { return fireRange; }
    int getRemainingMs() const { return remainingMs; }
    const std::vector<BlastCell>& getBlastCells() const { return blastCells; }

private:
    int x;
    int y;
    int tileSize;
    int remainingMs;
    int fireRange;
    bool exploding;
    int explosionRemainingMs;
    std::vector<BlastCell> blastCells;
};