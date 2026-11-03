#include "../src/Bomb.h"
#include "../src/Movement.h"
#include <cassert>
#include <iostream>

int main() {
	Bomb defaultBomb(0, 0, 32);
	assert(defaultBomb.getRemainingMs() == Bomb::DEFAULT_FUSE_MS);
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

	Bomb bomb(64, 96, 32, 5000);
	movement::Body body;
	body.x = 64;
	body.y = 32;
	auto empty = [](int, int) { return false; };
	auto bombPlatform = [&](movement::Body& player, float previousY) {
		if (movement::crossesPlatform(player, previousY, float(bomb.getX()),
				float(bomb.getY()), float(bomb.getTileSize()))) {
			player.y = float(bomb.getY()) - player.height;
			player.vy = 0;
			player.grounded = true;
		}
	};
	for (int i = 0; i < 100; ++i)
		movement::tick(body, 0, false, .008f, 32, empty, bombPlatform);
	assert(body.grounded && body.y == 64);

	movement::tick(body, 0, true, .008f, 32, empty, bombPlatform);
	assert(body.vy < 0 && !body.grounded);
	for (int i = 0; i < 150 && !body.grounded; ++i)
		movement::tick(body, 0, false, .008f, 32, empty, bombPlatform);
	assert(body.grounded && body.y == 64);

	std::cout << "PASS: bomb timer and one-way jumpable platform\n";
}
