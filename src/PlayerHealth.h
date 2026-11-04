#pragma once
#include <algorithm>

// Remaining lives include the current attempt; the HUD shows spare attempts (2,1,0).
struct PlayerHealth {
    enum { MAX_LIVES = 3, HURT_MS = 2000 };
    enum class Damage { Ignored, GoldHeartLost, LifeLost };

    int lives = MAX_LIVES;
    int invulnerableMs = 0;
    bool goldHeart = false;

    int spareLives() const { return std::max(0, lives - 1); }
    bool canBeHurt() const { return lives > 0 && invulnerableMs == 0; }
    bool visible() const { return invulnerableMs == 0 || (invulnerableMs / 100) % 2 == 0; }
    void update(int milliseconds) {
        invulnerableMs = std::max(0, invulnerableMs - std::max(0, milliseconds));
    }
    void collectGoldHeart() { goldHeart = true; }
    Damage takeHit() {
        if (!canBeHurt()) return Damage::Ignored;
        invulnerableMs = HURT_MS;
        if (goldHeart) {
            goldHeart = false;
            return Damage::GoldHeartLost;
        }
        --lives;
        return Damage::LifeLost;
    }
    void reset() { *this = PlayerHealth(); }
};
