#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include "TileMap.h"


using namespace std;


TileMap *TileMap::createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program)
{
	TileMap *map = new TileMap(levelFile, minCoords, program);
	
	return map;
}


TileMap::TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program)
 : vao(0), vbo(0), nTiles(0), mapOrigin(minCoords), shaderProgram(&program)
{
	if (!loadLevel(levelFile)) throw std::runtime_error("Invalid level: " + levelFile);
	prepareArrays(minCoords, program);
}

TileMap::~TileMap()
{
	free();
}


void TileMap::render() const
{
	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * nTiles);
	glDisable(GL_TEXTURE_2D);
}

void TileMap::free()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

bool TileMap::loadLevel(const string &levelFile)
{
    ifstream input(levelFile);
    if (!level.load(input)) return false;
    if (!tilesheet.loadFromFile(level.atlas, TEXTURE_PIXEL_FORMAT_RGBA)) return false;
    if (tilesheet.width() % level.atlasColumns || tilesheet.height() % level.atlasRows) return false;
    tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
    tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
    tilesheet.setMinFilter(GL_NEAREST);
    tilesheet.setMagFilter(GL_NEAREST);
    return true;
}

bool TileMap::isSolid(int x, int y) const { return level.solid(x, y); }

bool TileMap::isFoliage(int x, int y) const { return level.isFoliage(x,y); }

bool TileMap::destroyFoliage(const std::vector<glm::ivec2>& cells) {
    bool changed = false;
    for (const auto& cell : cells) changed = level.destroyFoliage(cell.x,cell.y) || changed;
    if (changed) prepareArrays(mapOrigin, *shaderProgram);
    return changed;
}
void TileMap::hitWaterfallHeads(const std::vector<glm::ivec2>& cells) {
    for (const auto& cell : cells) level.hitWaterfallHead(cell.x,cell.y);
}
void TileMap::update(int deltaMs) {
    if (level.updateWaterfalls(deltaMs)) prepareArrays(mapOrigin, *shaderProgram);
}

void TileMap::prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program)
{
    vector<float> vertices;
    nTiles = 0;
    auto quad = [&](int tile, float x, float y, float height = 1.f) {
        if (!tile || height <= 0.f) return;
        ++nTiles;
        const float px = minCoords.x + x * level.tileSize;
        const float py = minCoords.y + y * level.tileSize;
        const float size = float(level.blockSize);
        const int column = (tile-1) % level.atlasColumns;
        const int row = (tile-1) / level.atlasColumns;
        const float u0 = float(column) / level.atlasColumns;
        const float v0 = float(row) / level.atlasRows;
        const float u1 = float(column+1) / level.atlasColumns;
        const float v1 = (row+height) / level.atlasRows;
        const float bottom = py + size*height;
        const float points[] = {
            px,py,u0,v0, px+size,py,u1,v0, px+size,bottom,u1,v1,
            px,py,u0,v0, px+size,bottom,u1,v1, px,bottom,u0,v1
        };
        vertices.insert(vertices.end(), points, points+24);
    };
    for (int front = 0; front < 2; ++front) {
        const auto& layer = front ? level.foreground : level.background;
        for (int y=0; y<level.height; ++y) for (int x=0; x<level.width; ++x) {
            // Replace the original waterfall column with its animated geometry.
            if (level.isWaterfallSourceTile(x,y,front != 0)) {
                if (!front) quad(1,float(x),float(y));
                continue;
            }
            quad(layer[y*level.width+x],float(x),float(y));
        }
        for (const auto& fall : level.waterfalls) if (fall.foreground == (front != 0)) {
            quad(7,float(fall.x),float(fall.top));
            const float foot = fall.footRow();
            for (int y=fall.top+1; y<foot; ++y)
                quad(8,float(fall.x),float(y),std::min(1.f,foot-y));
            quad(9,float(fall.x),foot);
        }
    }
    if (vao == 0) glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    if (vbo == 0) glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.empty() ? nullptr : vertices.data(), GL_DYNAMIC_DRAW);
    posLocation = program.bindVertexAttribute("position", 2, 4*sizeof(float), 0);
    texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4*sizeof(float), (void *)(2*sizeof(float)));
}
