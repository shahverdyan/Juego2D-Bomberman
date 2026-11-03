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

// Arbusto (tile 10) en el fondo o en el primer plano.
bool TileMap::isFoliage(int x, int y) const
{
    if (x < 0 || y < 0 || x >= level.width || y >= level.height) return false;
    const size_t index = size_t(y) * level.width + x;
    return level.background[index] == 10 || level.foreground[index] == 10;
}

bool TileMap::destroyFoliage(const std::vector<glm::ivec2>& cells)
{
    bool destroyed = false;
    for (const glm::ivec2& cell : cells) {
        if (cell.x < 0 || cell.y < 0 || cell.x >= level.width || cell.y >= level.height)
            continue;
        const size_t index = size_t(cell.y) * level.width + cell.x;
        if (level.background[index] == 10) {
            level.background[index] = 1;
            destroyed = true;
        }
        if (level.foreground[index] == 10) {
            level.foreground[index] = 0;
            destroyed = true;
        }
    }
    if (destroyed) prepareArrays(mapOrigin, *shaderProgram);
    return destroyed;
}

void TileMap::prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program)
{
    vector<float> vertices;
    nTiles = 0;
    // Draw the full background first, then foreground. Both use the same atlas.
    for (const auto* layer : { &level.background, &level.foreground }) {
        for (int y = 0; y < level.height; ++y) {
            for (int x = 0; x < level.width; ++x) {
                int tile = (*layer)[y * level.width + x];
                if (!tile) continue;
                ++nTiles;
                const float px = minCoords.x + x * level.tileSize;
                const float py = minCoords.y + y * level.tileSize;
                const float size = float(level.blockSize);
                const int column = (tile - 1) % level.atlasColumns;
                const int row = (tile - 1) / level.atlasColumns;
                // Exact cell boundaries and nearest filtering preserve original pixels.
                const float u0 = float(column) / level.atlasColumns;
                const float v0 = float(row) / level.atlasRows;
                const float u1 = float(column + 1) / level.atlasColumns;
                const float v1 = float(row + 1) / level.atlasRows;
                const float quad[] = {
                    px,py,u0,v0, px+size,py,u1,v0, px+size,py+size,u1,v1,
                    px,py,u0,v0, px+size,py+size,u1,v1, px,py+size,u0,v1
                };
                vertices.insert(vertices.end(), quad, quad + 24);
            }
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
