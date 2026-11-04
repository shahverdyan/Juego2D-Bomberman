#pragma once
#include <algorithm>
#include <cmath>
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

    struct Waterfall {
        int x, top, bottom;
        bool foreground;
        int openingMs = -1; // -1 closed, 0..1000 retracting, 1000 open.
        float footRow() const {
            const float progress = std::max(0, openingMs) / 1000.f;
            return bottom - (bottom - top - 1) * progress;
        }
        bool isHead(int tx, int ty) const { return tx == x && ty == top; }
    };
    std::vector<Waterfall> waterfalls;
    struct HiddenItem { int x,y,type; };
    std::vector<HiddenItem> hiddenItems;
    bool hasItemSpawns = false;

    bool inBounds(int x, int y) const { return x >= 0 && y >= 0 && x < width && y < height; }
    bool isFoliage(int x, int y) const {
        if (!inBounds(x,y)) return false;
        const int i = y * width + x;
        return background[i] == 10 || foreground[i] == 10;
    }
    bool solid(int x, int y) const {
        if (!inBounds(x,y)) return true;
        if (collision[y * width + x] || isFoliage(x,y)) return true;
        for (const auto& fall : waterfalls)
            if (x == fall.x && y >= fall.top && y <= int(std::ceil(fall.footRow()))) return true;
        return false;
    }
    bool isWaterfallHead(int x, int y) const {
        for (const auto& fall : waterfalls) if (fall.isHead(x,y)) return true;
        return false;
    }
    bool isWaterfallSourceTile(int x, int y, bool front) const {
        for (const auto& fall : waterfalls)
            if (fall.foreground == front && fall.x == x && y >= fall.top && y <= fall.bottom) return true;
        return false;
    }
    void findWaterfalls() {
        waterfalls.clear();
        for (int front = 0; front < 2; ++front) {
            const auto& cells = front ? foreground : background;
            for (int y = 0; y < height; ++y) for (int x = 0; x < width; ++x) {
                if (cells[y*width+x] != 7) continue;
                int end = y+1;
                while (end < height && cells[end*width+x] == 8) ++end;
                if (end < height && cells[end*width+x] == 9)
                    waterfalls.push_back({x,y,end,front != 0,-1});
            }
        }
    }
    bool hitWaterfallHead(int x, int y) {
        for (auto& fall : waterfalls) if (fall.isHead(x,y) && fall.openingMs < 0) {
            fall.openingMs = 0; return true;
        }
        return false;
    }
    bool updateWaterfalls(int deltaMs) {
        bool changed = false;
        for (auto& fall : waterfalls) if (fall.openingMs >= 0 && fall.openingMs < 1000 && deltaMs > 0) {
            fall.openingMs = std::min(1000, fall.openingMs + deltaMs); changed = true;
        }
        return changed;
    }
    bool destroyFoliage(int x, int y) {
        if (!isFoliage(x,y)) return false;
        const int i = y*width+x;
        if (background[i] == 10) background[i] = 1;
        if (foreground[i] == 10) foreground[i] = 0;
        collision[i] = 0;
        return true;
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
        findWaterfalls();
        // The current player is 32x32; reject a spawn inside terrain or outside the map.
        const int span = (32 + tileSize - 1) / tileSize;
        for (int y = spawnY; y < spawnY + span; ++y)
            for (int x = spawnX; x < spawnX + span; ++x)
                if (solid(x, y)) return false;
        input >> std::ws;
        if (input.peek() != std::char_traits<char>::eof()) {
            int count;
            if (!(input >> tag >> count) || tag != "ITEMS" || count < 0 || count > width*height) return false;
            hasItemSpawns = true;
            for (int i=0; i<count; ++i) {
                HiddenItem item;
                if (!(input >> item.x >> item.y >> item.type) || item.type < 0 || item.type > 3 || !isFoliage(item.x,item.y)) return false;
                hiddenItems.push_back(item);
            }
            input >> std::ws;
            if (input.peek() != std::char_traits<char>::eof()) return false;
        }
        return true;
    }
};
