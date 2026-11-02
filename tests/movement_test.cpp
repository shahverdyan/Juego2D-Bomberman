#include "../src/Movement.h"
#include <cassert>
#include <iostream>

int main() {
    auto room = [](int x, int y) { return x <= 0 || x >= 20 || y <= 0 || y >= 10; };
    movement::Body b; b.x = 64; b.y = 64;
    for (int i=0; i<250; ++i) movement::tick(b, 0, false, .008f, 32, room);
    assert(b.grounded && std::abs(b.y-288.f)<.01f);
    movement::tick(b, 0, true, .008f, 32, room);
    assert(b.vy < 0 && !b.grounded);
    float apex = b.y;
    for (int i=0; i<150; ++i) {
        movement::tick(b, 0, false, .008f, 32, room);
        apex = std::min(apex, b.y);
    }
    assert(apex < 210 && b.grounded && b.y == 288);
    for (int i=0; i<500; ++i) movement::tick(b, 1, false, .008f, 32, room);
    assert(b.x == 608 && !movement::overlaps(b, 32, room));
    for (int i=0; i<600; ++i) movement::tick(b, -1, false, .008f, 32, room);
    assert(b.x == 32);
    b.x=64; b.y=40; b.vy=-440;
    movement::tick(b, 0, false, .04f, 32, room);
    assert(b.y == 32 && b.vy == 0); // Ceiling.
    b.y=64;
    movement::moveAxis(b, 1000, false, 32, room);
    assert(b.y == 288); // Large fall cannot cross the floor.
    auto platform = [](int x,int y) { return y>=10 || (y==6 && x>=3 && x<=5); };
    b.x=96; b.y=96; b.vy=0;
    for(int i=0;i<125;++i) movement::tick(b,0,false,.008f,32,platform);
    assert(b.y==160 && b.grounded);
    for(int i=0;i<200;++i) movement::tick(b,1,false,.008f,32,platform);
    assert(b.y==288 && b.grounded); // Walking off an edge falls to the floor.
    // One second of identical fixed updates, grouped into 25 or 125 renders.
    movement::Body a, c; a.x=c.x=64; a.y=c.y=288;
    for(int frame=0;frame<25;++frame) for(int tick=0;tick<5;++tick)
        movement::tick(a,1,false,.008f,32,room);
    for(int frame=0;frame<125;++frame) movement::tick(c,1,false,.008f,32,room);
    assert(std::abs(a.x-c.x)<.001f && std::abs(a.x-224.f)<.01f);
    std::cout << "PASS: floor, jump, walls, ceiling, tunnelling, platform, edge, fixed timing\n";
}
