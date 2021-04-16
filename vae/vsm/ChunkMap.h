//
// Created by protocol on 4/14/21.
//

#ifndef BOOSTTESTING_CHUNKMAP_H
#define BOOSTTESTING_CHUNKMAP_H


#include <vector>
#include <iostream>
#include <list>
#include "TestVisualize.h"

namespace vae {
    namespace vsm {
        namespace chunk {
class Map;
class Tile;
class Node;
typedef int coordType;

class Tile{
    //placeholder
    int tileNum;
};

class NodeList{
    Node &node;
    int *pos;
    NodeList *p, *n;
public:
    NodeList(Node &parent): node(parent), pos(pos){
    }
};

class Node{
    NodeList x, y;
public:
    Node(): x(*this), y(*this){

    }
};

class Chunklet{
    Tile **tiles;
public:
    const coordType x;
    const coordType y;
    Chunklet(Map &parent, int chunkSize, coordType x, coordType y): x(x), y(y){
        tiles = new Tile*[chunkSize];
        for(int a = 0; a < chunkSize; a++)
            tiles[a] = new Tile[chunkSize];
    }
};

/**
 * Holds Nodes
 */
class Map {
private:
    int addChunk(Chunklet* dis, coordType x, coordType y){
        //if(map[x].at(y) == nullptr)
        //    map[x] = new std::vector<Chunklet*>();
        map[x][y] = dis;
        chunks.push_front(dis);
    }
public:
    const int chunkSize;
    const int chunkBitShift;
    std::map<coordType, std::map<coordType, Chunklet*>> map;
    std::list<Chunklet*> chunks;

    Map(int chunkSize): chunkSize(chunkSize), chunkBitShift(chunkSize>>1){
    }
    void pregenChunks(coordType xOffset, coordType yOffset, int size){
        for(int ix = 0 ; ix < size ; ix += chunkSize){
            for(int iy = 0 ; iy < size ; iy += chunkSize){
                addChunk(new Chunklet(*this, chunkSize, ix + xOffset, iy + yOffset), ix + xOffset, iy + yOffset);
            }
        }
    }
    void insert(Node *node){
    }

    void draw(TestVisualize &testVisualize){
        for(auto i = chunks.begin(); i != chunks.end(); i++)
            for(int x = 0; x < chunkSize; x++)
                for(int y = 0; y < chunkSize; y++)
                    testVisualize.drawPoint(x, y);
    }
};

/**
 * High-level chunk generator-should be able to generate a single tile
 */
class ChunkMapComposer{

};

        } //namespace chunk
    } //namespace vsm
} //namespace vae

#endif //BOOSTTESTING_CHUNKMAP_H
