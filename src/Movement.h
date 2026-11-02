#pragma once
#include <algorithm>
#include <cmath>

// Grid collision without OpenGL: shared by the game and physics tests.
namespace movement {
struct Body {
    float x = 64, y = 672, vx = 0, vy = 0;
    float width = 32, height = 32;
    bool grounded = false;
};
template<class Solid>
bool overlaps(const Body& b, int tileSize, Solid solid) {
    const int left = int(std::floor(b.x / tileSize));
    const int right = int(std::floor((b.x + b.width - 0.001f) / tileSize));
    const int top = int(std::floor(b.y / tileSize));
    const int bottom = int(std::floor((b.y + b.height - 0.001f) / tileSize));
    for (int y = top; y <= bottom; ++y)
        for (int x = left; x <= right; ++x)
            if (solid(x, y)) return true;
    return false;
}
template<class Solid>
void moveAxis(Body& b, float distance, bool horizontal, int tileSize, Solid solid) {
    // One pixel maximum per substep prevents tunnelling.
    const int steps = std::max(1, int(std::ceil(std::abs(distance))));
    const float step = distance / steps;
    for (int i = 0; i < steps; ++i) {
        float& position = horizontal ? b.x : b.y;
        position += step;
        if (!overlaps(b, tileSize, solid)) continue;
        if (step > 0) {
            const float size = horizontal ? b.width : b.height;
            position = std::floor((position + size - 0.001f) / tileSize) * tileSize - size;
        } else if (step < 0) {
            position = (std::floor(position / tileSize) + 1) * tileSize;
        }
        if (horizontal) b.vx = 0;
        else { b.vy = 0; if (step > 0) b.grounded = true; }
        break;
    }
}
template<class Solid>
void tick(Body& b, int direction, bool jumpPressed, float seconds, int tileSize, Solid solid) {
    b.vx = direction * 160.f;
    if (jumpPressed && b.grounded) { b.vy = -440.f; b.grounded = false; }
    b.vy = std::min(600.f, b.vy + 1100.f * seconds);
    moveAxis(b, b.vx * seconds, true, tileSize, solid);
    b.grounded = false;
    moveAxis(b, b.vy * seconds, false, tileSize, solid);
}
}
