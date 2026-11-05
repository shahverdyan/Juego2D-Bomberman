#include "../src/Bomb.h"
#include "../src/Movement.h"
#include <cassert>
#include <iostream>

int main() {
    Bomb pulse(0,0,32);
    assert(pulse.getBombFrame()==2);
    pulse.update(220); assert(pulse.getBombFrame()==1);
    pulse.update(220); assert(pulse.getBombFrame()==0);
    pulse.update(220); assert(pulse.getBombFrame()==1);
    for(int i=0;i<300;++i) { pulse.update(8); assert(pulse.getBombFrame()>=0 && pulse.getBombFrame()<3); }
    Bomb blast(0,0,32,0);
    assert(blast.update(0) && blast.getExplosionFrame()==0);
    blast.update(67); assert(blast.getExplosionFrame()==1);
    blast.update(67); assert(blast.getExplosionFrame()==2);
    blast.update(66); assert(blast.getExplosionFrame()==2);
    blast.update(67); assert(blast.getExplosionFrame()==1);
    blast.update(67); assert(blast.getExplosionFrame()==0);
    blast.update(65); assert(!blast.isExpired());
    blast.update(1); assert(blast.isExpired());
    Bomb fresh(0,0,32);
    assert(fresh.getBombFrame()==2 && blast.getExplosionFrame()==0);
	Bomb defaultBomb(0, 0, 32);
	assert(defaultBomb.getRemainingMs() == Bomb::DEFAULT_FUSE_MS);
	assert(!defaultBomb.update(1999) && defaultBomb.update(1));
	assert(defaultBomb.getTileSize() == 32);
	assert(defaultBomb.getFireRange() == Bomb::DEFAULT_FIRE_RANGE);
	Bomb upgradedBomb(0, 0, 32, Bomb::DEFAULT_FUSE_MS, 3);
	assert(upgradedBomb.getFireRange() == 3);

	Bomb timerBomb(64, 96, 32, 1000);
	assert(timerBomb.getX() == 64 && timerBomb.getY() == 96 && timerBomb.getTileSize() == 32);
	assert(timerBomb.getRemainingMs() == 1000 && !timerBomb.isExpired());
	timerBomb.update(400);
	assert(timerBomb.getRemainingMs() == 600 && !timerBomb.isExpired());
	timerBomb.update(-100);
	assert(timerBomb.getRemainingMs() == 600);
	assert(timerBomb.update(700));
	assert(timerBomb.getRemainingMs() == 0 && timerBomb.isExploding() && !timerBomb.isExpired());
	timerBomb.setBlastCells({ { 2, 3 }, { 3, 3 } });
	assert(timerBomb.getBlastCells().size() == 2);
	timerBomb.update(Bomb::DEFAULT_EXPLOSION_MS - 1);
	assert(!timerBomb.isExpired());
	timerBomb.update(1);
	assert(timerBomb.isExpired());

    // Position remains in the current tile while the centre stays inside it.
    for (float offset : {-15.9f,-8.f,-1.f,0.f,1.f,8.f,15.9f})
        assert(Bomb::snapToBodyCell(64+offset,32,32)==64);
    assert(Bomb::snapToBodyCell(64+16.1f,32,32)==96);
    assert(Bomb::snapToBodyCell(64-16.1f,32,32)==32);

    Bomb bomb(64,96,32,5000);
    auto floor = [](int,int y) { return y>=4; };
    auto solid = [&](int x,int y) { return floor(x,y) || bomb.blocksPlayerTile(x,y); };
    movement::Body body;
    body.x=64; body.y=0;
    for(int i=0;i<200;++i) movement::tick(body,0,false,.008f,32,solid);
    assert(body.grounded && body.y==64); // Landing on the top still works.
    movement::tick(body,0,true,.008f,32,solid);
    assert(body.vy<0);
    for(int i=0;i<200;++i) movement::tick(body,0,false,.008f,32,solid);
    assert(body.grounded && body.y==64);

    body.x=0; body.y=96; body.vy=0;
    for(int i=0;i<200;++i) movement::tick(body,1,false,.008f,32,solid);
    assert(body.x==32 && body.y==96); // Left side.
    body.x=128; body.y=96; body.vy=0;
    for(int i=0;i<200;++i) movement::tick(body,-1,false,.008f,32,solid);
    assert(body.x==96 && body.y==96); // Right side.

    body.x=64; body.y=160; body.vy=0; body.grounded=true;
    auto lowerFloor = [&](int x,int y) { return y>=6 || bomb.blocksPlayerTile(x,y); };
    bool touchedUnderside=false;
    for(int i=0;i<100;++i) {
        movement::tick(body,0,i==0,.008f,32,lowerFloor);
        assert(body.y>=128);
        if(body.y==128 && body.vy==0) touchedUnderside=true;
    }
    assert(touchedUnderside); // Cannot jump through a bomb from below.

    body.x=64; body.y=96; body.vy=0; body.grounded=true;
    bomb.allowOwnerExit(body.x,body.y,body.width,body.height);
    assert(bomb.canOwnerExit());
    for(int i=0;i<100;++i) {
        bomb.updateOwnerExit(body.x,body.y,body.width,body.height);
        movement::tick(body,1,false,.008f,32,solid);
        bomb.updateOwnerExit(body.x,body.y,body.width,body.height);
    }
    assert(body.x>=96 && !bomb.canOwnerExit());
    for(int i=0;i<200;++i) movement::tick(body,-1,false,.008f,32,solid);
    assert(body.x==96); // After leaving, re-entering is blocked.

    body.x=64; body.y=96; body.vy=0; body.grounded=true;
    bomb.allowOwnerExit(body.x,body.y,body.width,body.height);
    for(int i=0;i<200;++i) {
        movement::tick(body,0,i==0,.008f,32,solid);
        bomb.updateOwnerExit(body.x,body.y,body.width,body.height);
    }
    assert(body.grounded && body.y==64 && !bomb.canOwnerExit());

    bomb.update(5000);
    assert(bomb.isExploding() && !bomb.blocksPlayerTile(2,3));
    body.x=0; body.y=96; body.vy=0;
    for(int i=0;i<100;++i) movement::tick(body,1,false,.008f,32,solid);
    assert(body.x>96); // No solid bomb remains once it explodes.
    std::cout << "PASS: bomb timer, centred placement, top/sides/underside, owner exit and explosion collision removal\n";
}
