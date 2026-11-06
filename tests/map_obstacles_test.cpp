#include "../src/LevelData.h"
#include "../src/Movement.h"
#include "../src/Bomb.h"
#include "../src/ViewConfig.h"
#include <cassert>
#include <fstream>
#include <iostream>

int main() {
    LevelData level;
    std::ifstream input("src/levels/level05.txt");
    assert(level.load(input));
    assert(level.hasItemSpawns && !level.hiddenItems.empty());
    const auto itemCount = level.hiddenItems.size();
    assert(level.waterfalls.size()==3);
    assert(level.isFoliage(3,24) && level.solid(3,24));
    auto solid=[&](int x,int y) { return level.solid(x,y); };
    auto reactive=[&](int x,int y) { return level.isFoliage(x,y) || level.isWaterfallHead(x,y); };
    assert(Bomb::blastDistance(2,24,1,0,5,solid,reactive)==1);
    movement::Body body; body.x=64; body.y=24*32; body.grounded=true;
    for(int i=0;i<50;++i) movement::tick(body,1,false,.008f,32,solid);
    assert(body.x==64); // The start bush is now a wall.
    assert(level.destroyFoliage(3,24));
    assert(!level.solid(3,24) && !level.isFoliage(3,24));
    assert(!level.destroyFoliage(3,24));
    for(int i=0;i<10;++i) movement::tick(body,1,false,.008f,32,solid);
    assert(body.x>64);

    // Long central waterfall: only a hit on the head can retract it.
    const auto original=level.waterfalls[1];
    const int x=original.x, top=original.top, bottom=original.bottom;
    assert(level.solid(x,top) && level.solid(x,bottom));
    assert(!level.hitWaterfallHead(x,top+1));
    assert(!level.updateWaterfalls(1000));
    assert(Bomb::blastDistance(x+1,top,-1,0,1,solid,reactive)==1);
    assert(Bomb::blastDistance(x+1,top+1,-1,0,1,solid,reactive)==0);
    assert(level.hitWaterfallHead(x,top));
    assert(!level.hitWaterfallHead(x,top));
    level.updateWaterfalls(500);
    assert(std::abs(level.waterfalls[1].footRow()-(top+1+bottom)/2.f)<.001f);
    assert(!level.solid(x,bottom));
    level.updateWaterfalls(499);
    assert(level.waterfalls[1].openingMs==999);
    level.updateWaterfalls(20);
    assert(level.waterfalls[1].openingMs==1000);
    assert(level.waterfalls[1].footRow()==top+1);
    assert(level.solid(x,top) && level.solid(x,top+1));
    for(int y=top+2;y<=bottom;++y)
        assert(level.solid(x,y)==bool(level.collision[y*level.width+x] || level.isFoliage(x,y)));
    assert(!level.updateWaterfalls(100));
    std::ifstream reset("src/levels/level05.txt");
    assert(level.load(reset) && level.waterfalls[1].openingMs==-1);
    assert(level.solid(x,bottom));
    assert(level.hasItemSpawns && level.hiddenItems.size()==itemCount);
    for(const auto& item : level.hiddenItems) assert(level.isFoliage(item.x,item.y));

    const auto camera=view::follow(64,768,1536,832,32);
    assert(camera.width==320 && camera.height==288);
    assert(view::WINDOW_HEIGHT-view::HUD_HEIGHT==576 && view::WINDOW_WIDTH==640);
    assert(camera.x>=0 && camera.y>=0 && camera.y+camera.height<=832);
    std::cout << "PASS: solid bushes, destructive fire, items, waterfall hit/retraction/reset and 10x9 camera\n";
}
