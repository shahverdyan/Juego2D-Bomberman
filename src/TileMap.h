#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
#include <vector>
#include "Texture.h"
#include "LevelData.h"
#include "ShaderProgram.h"


// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.


class TileMap
{

private:
	TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap *createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);

	~TileMap();

	void render() const;
	void free();
	
	int getTileSize() const { return level.tileSize; }

	bool isSolid(int x, int y) const;
	bool destroyFoliage(const std::vector<glm::ivec2>& cells);
    glm::vec2 getSpawn() const { return glm::vec2(level.spawnX * level.tileSize, level.spawnY * level.tileSize); }
    glm::ivec2 getPixelSize() const { return glm::ivec2(level.width, level.height) * level.tileSize; }

private:
	bool loadLevel(const string &levelFile);
	void prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program);

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	int nTiles;
	glm::vec2 mapOrigin;
	ShaderProgram* shaderProgram;
	LevelData level;
    Texture tilesheet;

};


#endif // _TILE_MAP_INCLUDE


