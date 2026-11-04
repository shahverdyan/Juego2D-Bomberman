#include "../src/PlayerHealth.h"
#include <cassert>
#include <iostream>

int main() {
    PlayerHealth health;
    assert(health.lives==3 && health.spareLives()==2 && !health.goldHeart);
    assert(health.takeHit()==PlayerHealth::Damage::LifeLost);
    assert(health.lives==2 && health.spareLives()==1);
    health.update(2000);
    health.collectGoldHeart();
    health.collectGoldHeart(); // A second pickup does not add or stack lives.
    assert(health.lives==2 && health.goldHeart);
    assert(health.takeHit()==PlayerHealth::Damage::GoldHeartLost);
    assert(health.lives==2 && health.spareLives()==1 && !health.goldHeart);
    assert(health.invulnerableMs==2000 && !health.canBeHurt());
    assert(health.takeHit()==PlayerHealth::Damage::Ignored && health.lives==2);
    assert(health.visible());
    health.update(100); assert(!health.visible());
    health.update(100); assert(health.visible());
    health.update(1799); assert(!health.canBeHurt());
    health.update(1); assert(health.canBeHurt() && health.visible());
    assert(health.takeHit()==PlayerHealth::Damage::LifeLost);
    assert(health.lives==1 && health.spareLives()==0); // Last attempt, still alive.
    health.update(2000);
    assert(health.takeHit()==PlayerHealth::Damage::LifeLost && health.lives==0);
    health.update(2000);
    assert(!health.canBeHurt());
    assert(health.takeHit()==PlayerHealth::Damage::Ignored && health.lives==0);
    health.reset();
    assert(health.lives==3 && health.spareLives()==2 && !health.goldHeart && health.canBeHurt());
    std::cout << "PASS: 2-1-0 spare lives, golden shield, no extra life, two-second blink/immunity and reset\n";
}
