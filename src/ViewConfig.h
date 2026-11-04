#pragma once
#include <algorithm>
namespace view {
    enum { COLUMNS=10, ROWS=9, WINDOW_WIDTH=640, HUD_HEIGHT=48,
           WINDOW_HEIGHT=WINDOW_WIDTH*ROWS/COLUMNS+HUD_HEIGHT };
    struct Camera { float x,y,width,height; };
    inline Camera follow(float x, float y, int mapWidth, int mapHeight, int tileSize) {
        const float width=float(COLUMNS*tileSize), height=float(ROWS*tileSize);
        return {std::max(0.f,std::min(x+tileSize/2.f-width/2.f,std::max(0.f,mapWidth-width))),
                std::max(0.f,std::min(y+tileSize/2.f-height/2.f,std::max(0.f,mapHeight-height))),width,height};
    }
}
