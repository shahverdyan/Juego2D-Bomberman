#include "../src/LevelData.h"
#include "../src/Movement.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>

int main() {
    std::ifstream input("src/levels/level01.txt");
    const std::string text((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    std::istringstream stream(text);
    LevelData level;
    assert(level.load(stream));
    assert(level.width == 48 && level.height == 26);
    auto solid = [&](int x, int y) { return level.solid(x, y); };
    assert(!solid(9, 12)); // Waterfall decoration is traversable.
    assert(solid(7, 16)); // Column below it is solid.
    assert(!solid(43, 7) && solid(43, 8)); // Door marker and its platform.
    assert(solid(-1, 2) && solid(level.width, 2));
    LevelData bad;
    std::istringstream truncated(text.substr(0, text.size() / 2));
    assert(!bad.load(truncated));
    std::string invalid = text;
    invalid.replace(invalid.find("SPAWN 2 24"), 10, "SPAWN 0 0");
    std::istringstream blockedSpawn(invalid);
    assert(!bad.load(blockedSpawn));
    invalid = text;
    invalid.insert(invalid.find("BACKGROUND\n") + 11, "999 ");
    std::istringstream badTile(invalid);
    assert(!bad.load(badTile));

    // Search actual physics trajectories between standing positions every four pixels.
    // This verifies a route from spawn to the upper marker without bombs or teleporting.
    using Point = std::pair<int,int>;
    std::map<Point, int> index;
    std::vector<movement::Body> nodes;
    for (int y = 0; y < level.height * 32; y += 32) {
        for (int x = 0; x < level.width * 32; x += 4) {
            movement::Body body; body.x = float(x); body.y = float(y);
            if (movement::overlaps(body, 32, solid)) continue;
            body.y += .1f;
            if (!movement::overlaps(body, 32, solid)) continue;
            body.y = float(y); body.grounded = true;
            index[{x,y}] = int(nodes.size()); nodes.push_back(body);
        }
    }
    auto start = index.find({level.spawnX*32,level.spawnY*32});
    assert(start != index.end());
    std::vector<bool> visited(nodes.size());
    std::queue<int> pending; pending.push(start->second); visited[start->second] = true;
    std::vector<std::vector<int>> reverseEdges(nodes.size());
    int originId = -1;
    auto add = [&](Point point) {
        auto found = index.find(point);
        if (found != index.end()) reverseEdges[found->second].push_back(originId);
        if (found != index.end() && !visited[found->second]) {
            visited[found->second] = true; pending.push(found->second);
        }
    };
    while (!pending.empty()) {
        originId = pending.front();
        const movement::Body origin = nodes[originId]; pending.pop();
        for (int direction : {-1, 1}) {
            // Include walking off edges: falling is a valid return route.
            movement::Body walk = origin;
            for (int tick=0; tick<400; ++tick) {
                movement::tick(walk, direction, false, .008f, 32, solid);
                if (walk.grounded && (std::abs(walk.x-origin.x)>=4.f || walk.vx==0)) {
                    int x = int(std::round(walk.x/4.f))*4;
                    movement::Body rounded = walk;
                    movement::moveAxis(rounded, float(x)-walk.x, true, 32, solid);
                    if (std::abs(rounded.x-x)<.01f) add({x,int(std::round(walk.y))});
                    break;
                }
            }
        }

        for (int direction : {-1, 0, 1}) {
            movement::Body body = origin;
            for (int tick=0; tick<400; ++tick) {
                movement::tick(body, direction, tick==0, .008f, 32, solid);
                assert(!movement::overlaps(body, 32, solid));
                if (tick>0 && body.grounded) {
                    // Connect only via a short collision-free movement to a sample.
                    int x = int(std::round(body.x/4.f))*4;
                    movement::Body rounded = body;
                    movement::moveAxis(rounded, float(x)-body.x, true, 32, solid);
                    if (std::abs(rounded.x-x)<.01f) add({x,int(std::round(body.y))});
                    break;
                }
            }
        }
    }
    auto target = index.find({43*32,7*32});
    assert(target != index.end() && visited[target->second]);
    // Both sides of the lower route and the high loop can be visited.
    for (Point point : {Point{11*32,18*32}, Point{26*32,17*32}, Point{38*32,14*32}, Point{6*32,9*32}, Point{2*32,17*32}, Point{30*32,3*32}}) {
        assert(index.count(point) && visited[index[point]]);
    }
    std::vector<bool> canReturn(nodes.size());
    pending.push(start->second); canReturn[start->second] = true;
    while (!pending.empty()) {
        const int id = pending.front(); pending.pop();
        for (int previous : reverseEdges[id]) if (!canReturn[previous]) {
            canReturn[previous] = true; pending.push(previous);
        }
    }
    int reachable = 0, trapped = 0;
    for (size_t i=0; i<nodes.size(); ++i) if (visited[i]) {
        ++reachable;
        if (!canReturn[i]) {
            if (++trapped <= 8) std::cerr << "No return from tile " << nodes[i].x/32 << "," << nodes[i].y/32 << "\n";
        }
    }
    std::cout << "Reachable standing samples: " << reachable << "; without return: " << trapped << "\n";
    assert(trapped == 0);
    std::cout << "PASS: level parsing, invalid input, decoration/collision separation, spawn and playable routes\n";
}
