#pragma once
#include <algorithm>
#include <cmath>
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
        : x(x), y(y), tileSize(tileSize), fuseDurationMs(std::max(0, fuseDurationMs)), remainingMs(std::max(0, fuseDurationMs)),
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

    // Select the tile containing the player's centre, not the leading edge.
    static int snapToBodyCell(float position, float extent, int cellSize) {
        return int(std::floor((position + extent * .5f) / cellSize)) * cellSize;
    }
    bool overlapsPlayer(float px, float py, float width, float height) const {
        return px < x + tileSize && px + width > x && py < y + tileSize && py + height > y;
    }
    // A newly planted bomb permits its owner to exit once, preventing entrapment.
    void allowOwnerExit(float px, float py, float width, float height) {
        ownerExitAllowed = overlapsPlayer(px,py,width,height);
    }
    void updateOwnerExit(float px, float py, float width, float height) {
        if (ownerExitAllowed && !overlapsPlayer(px,py,width,height)) ownerExitAllowed = false;
    }
    bool canOwnerExit() const { return ownerExitAllowed; }
    bool blocksPlayerTile(int tx, int ty) const {
        return !exploding && !ownerExitAllowed && tx == x / tileSize && ty == y / tileSize;
    }

    // Include the first reactive obstacle, but stop fire before ordinary terrain.
    template<class Solid, class Reactive>
    static int blastDistance(int x, int y, int dx, int dy, int range, Solid solid, Reactive reactive) {
        int distance=0;
        for (int step=1; step<=range; ++step) {
            const int tx=x+dx*step, ty=y+dy*step;
            if (reactive(tx,ty)) { distance=step; break; }
            if (solid(tx,ty)) break;
            distance=step;
        }
        return distance;
    }

    // Every bomb owns its visual phase: new bombs never reset an existing explosion.
    int getBombFrame() const {
        const int age=fuseDurationMs-remainingMs;
        const int mediumStart=std::max(0,fuseDurationMs-1500);
        const int fastStart=std::max(0,fuseDurationMs-500);
        const float phase=std::min(age,mediumStart)/220.f
            + std::min(std::max(age-mediumStart,0),fastStart-mediumStart)/140.f
            + std::max(age-fastStart,0)/80.f;
        const int pulse[4]={2,1,0,1};
        return pulse[int(phase)%4];
    }
    int getExplosionFrame() const {
        if (!exploding) return 0;
        const int age=DEFAULT_EXPLOSION_MS-explosionRemainingMs;
        const int sequence[6]={0,1,2,2,1,0};
        return sequence[std::min(5,age*6/DEFAULT_EXPLOSION_MS)];
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
    int fuseDurationMs;
    int remainingMs;
    int fireRange;
    bool ownerExitAllowed = false;
    bool exploding;
    int explosionRemainingMs;
    std::vector<BlastCell> blastCells;
};