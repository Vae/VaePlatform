//
// Created by protocol on 4/14/21.
//

#ifndef BOOSTTESTING_BASICMAP_H
#define BOOSTTESTING_BASICMAP_H

#include "TestVisualize.h"

namespace vae {
    namespace vsm {
        namespace basic {
class Map;
class Entity{

};
class Tile{
private:
    int tilenum;
public:
    Tile(){
        this->tilenum = 0;
    }
    int getTileNum() { return tilenum; }

};

class Map {
    Tile **grid;
    const int width, height;
public:
    Map(int width, int height): width(width), height(height){
        grid = new Tile*[height];
        for(int a = 0; a < height; a++)
            grid[a] = new Tile[width];
    }

    Tile &getTile(int x, int y){
        return grid[x][y];
    }

    void draw(TestVisualize &testVisualize){
        for(int x = 0; x < width; x++)
            for(int y = 0; y < height; y++)
                testVisualize.drawPoint(x, y);
    }
};

        } //namespace basic
    } //namespace vsm
} //namespace vae

#endif //BOOSTTESTING_BASICMAP_H
