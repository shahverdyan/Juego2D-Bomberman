#pragma once
#include <istream>
#include <sstream>
#include <string>
#include <vector>

// CPU-only level format: decorative layers and collision are independent.
struct LevelData {
    int width = 0, height = 0, tileSize = 0, blockSize = 0;
    int atlasColumns = 0, atlasRows = 0, spawnX = 0, spawnY = 0;
    std::string atlas;
    std::vector<int> background, foreground;
    std::vector<unsigned char> collision;

    bool solid(int x, int y) const {
        return x < 0 || y < 0 || x >= width || y >= height || collision[y * width + x] != 0;
    }

    bool load(std::istream& input) {
        *this = LevelData();
        std::string line, tag;
        auto nextLine = [&]() {
            if (!std::getline(input, line)) return false;
            if (!line.empty() && line.back() == '\r') line.pop_back();
            return true;
        };
        if (!nextLine() || line != "TILEMAP2") return false;
        if (!nextLine() || !(std::istringstream(line) >> width >> height)) return false;
        if (!nextLine() || !(std::istringstream(line) >> tileSize >> blockSize)) return false;
        if (!nextLine() || line.empty()) return false;
        atlas = line;
        if (!nextLine() || !(std::istringstream(line) >> atlasColumns >> atlasRows)) return false;
        if (width < 3 || height < 3 || width > 512 || height > 512 ||
            tileSize < 1 || tileSize > 256 || blockSize != tileSize ||
            atlasColumns < 1 || atlasRows < 1 || atlasColumns > 256 || atlasRows > 256) return false;
        if (!nextLine() || !(std::istringstream(line) >> tag >> spawnX >> spawnY) || tag != "SPAWN") return false;
        if (spawnX < 0 || spawnY < 0 || spawnX >= width || spawnY >= height) return false;
        auto layer = [&](const char* name, std::vector<int>& cells) {
            if (!nextLine() || line != name) return false;
            for (int y = 0; y < height; ++y) {
                if (!nextLine()) return false;
                std::istringstream row(line);
                for (int x = 0; x < width; ++x) {
                    int id;
                    if (!(row >> id) || id < 0 || id > atlasColumns * atlasRows) return false;
                    cells.push_back(id);
                }
                row >> std::ws;
                if (!row.eof()) return false;
            }
            return true;
        };
        if (!layer("BACKGROUND", background) || !layer("FOREGROUND", foreground)) return false;
        if (!nextLine() || line != "COLLISION") return false;
        for (int y = 0; y < height; ++y) {
            if (!nextLine() || line.size() != size_t(width)) return false;
            for (char c : line) {
                if (c != '0' && c != '1') return false;
                collision.push_back(static_cast<unsigned char>(c - '0'));
            }
        }
        // The current player is 32x32; reject a spawn inside terrain or outside the map.
        const int span = (32 + tileSize - 1) / tileSize;
        for (int y = spawnY; y < spawnY + span; ++y)
            for (int x = spawnX; x < spawnX + span; ++x)
                if (solid(x, y)) return false;
        return true;
    }
};
