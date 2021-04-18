//
// Created by protocol on 4/14/21.
//

#ifndef BOOSTTESTING_CHUNKMAP_H
#define BOOSTTESTING_CHUNKMAP_H


#include <vector>
#include <iostream>
#include <list>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/noncopyable.hpp>

#include "TestVisualize.h"
#include "../react/Action.h"
#include "../../olib/concurrentqueue.h"

namespace vae {
    namespace vsm {
        namespace chunk {
class Map;
class Tile;
class Node;
class ChunkMapComposer;

typedef int coordType;
typedef std::string MapId;

class Tile: private boost::noncopyable{
    //placeholder
    int tileNum;
};

class NodeList: private boost::noncopyable{
    Node &node;
    coordType pos;
    NodeList *p, *n;
public:
    NodeList(Node &parent): node(parent), pos(0){
    }
    coordType getPos() const {return pos;}
    bool setPos(coordType to);
};


//Consumer
class Viewport: private boost::noncopyable{
    int width, height;
    coordType x, y;
public:
    moodycamel::ConcurrentQueue<vae::react::Action::Ptr> action_queue;
    typedef std::shared_ptr<Viewport> Ptr;
    Viewport(): width(3), height(3), x(0), y(0){}

    void cycle(float timestamp){
        while(action_queue.size_approx() > 0){
            //TODO: do observations
        }
    }

    int getWidth() const {return width;}
    void setWidth(int width) {Viewport::width = width;}
    int getHeight() const {return height;}
    void setHeight(int height) {Viewport::height = height;}
    coordType getX() const {return x;}
    void setX(coordType x) {Viewport::x = x;}
    coordType getY() const {return y;}
    void setY(coordType y) {Viewport::y = y;}
};



class Chunklet: private boost::noncopyable{
    Tile **tiles;

    /**
     * Since I want to try and have each Chunklet be on its own thread, they'll have to maintain their own lists.
     */
    std::list<Node*> nodes;
    std::list<Viewport*> viewports;

    //moodycamel::ConcurrentQueue<vae::react::Action::Ptr> action_queue;
public:
    typedef std::shared_ptr<Chunklet> Ptr;
    const int chunk_x;
    const int chunk_y;
    const int x_min;
    const int y_min;
    const int x_max;
    const int y_max;

    Chunklet(Map &parent, int chunkSize, int x, int y):
            chunk_x(x),
            chunk_y(y),
            x_min(x * chunkSize),
            y_min(y * chunkSize),
            x_max(x * chunkSize + (chunkSize - 1)),
            y_max(y * chunkSize + (chunkSize - 1)){
        tiles = new Tile*[chunkSize];
        for(int a = 0; a < chunkSize; a++)
            tiles[a] = new Tile[chunkSize];
    }
    ~Chunklet(){
        std::cout << "Chunklet lost " << x_min << " " << y_min << std::endl;
    }

    void insert(vae::react::Action::Ptr action){
        for(auto i : viewports)
            i->action_queue.enqueue(action);
    }

    bool insert(Viewport *viewport){
        viewports.push_back(viewport);
        return false;
    }
    bool insert(Node *node);
    void remove(Node *node){
        //TODO: Inform viewports that the node left
        nodes.remove(node);

        std::cout << "Removed node." << std::endl;
    }
};

//Producer
class Node: private boost::noncopyable {
    friend class ChunkMapComposer;
    NodeList x, y;
    Chunklet::Ptr chunk;
    std::shared_ptr<Map> map;
    MapId mapId;
public:
    typedef std::shared_ptr<Node> Ptr;
    Node(): x(*this), y(*this){
        vae::react::Action a;
    }
    void say(std::string dis){}

    void setMap(std::shared_ptr<Map> to){ this->map = to; }

    NodeList& getXNode() { return x; }
    NodeList& getYNode() { return y; }
    Chunklet::Ptr getChunklet() const { return chunk; }
    std::shared_ptr<Map> getMap() const { return map; }
    MapId getMapId() const { return mapId; }
    void setChunklet(Chunklet::Ptr to) { chunk = to; }
};
/**
 * Holds Nodes
 */
class Map: private boost::noncopyable {
    friend class ChunkMapComposer;
private:
    int addChunk(Chunklet::Ptr dis, coordType x, coordType y){
        chunks.push_back(dis);
        map[x].emplace(y, dis);
        return 0;
    }
    std::map<coordType, std::map<coordType, Chunklet::Ptr>> map;
    std::list<Chunklet::Ptr> chunks;
    std::list<std::reference_wrapper<Node>> nodes;
    std::list<std::reference_wrapper<Viewport>> viewports;
    /**
     *
     * @param x
     * @param y
     * @return pointer to the loaded chunk
     */
    Chunklet::Ptr loadChunk(int x, int y){
        std::cout << "New chunk " << x << " " << y << std::endl;
        return Chunklet::Ptr(new Chunklet(*this, chunkSize, x, y));
        //return std::make_shared(*this, chunkSize, x, y);
    }

/**
 * When a node is insert, based on its location and size, it needs to be added to all consumers of that location.
 * @param node
 * @returns bool
 *  If the node insertion failed, true will return, else false for success.
 */
    bool insert(Node &node){
        //TODO: Ensure the node is in bounds defined by the Map shape
        //Ensure the chunk is loaded; if not, load it.
        int chunk_x = translateToChunk(node.getXNode().getPos());
        int chunk_y = translateToChunk(node.getYNode().getPos());

        Chunklet::Ptr dest = getChunklet(chunk_x, chunk_y);

        if(dest == NULL || dest->insert(&node)) {
            std::cout << "Failed to insert node." << std::endl;
            return true;
        }
        node.setChunklet(dest);
        nodes.push_back(node);
        return false;
    }
public:
    typedef std::shared_ptr<Map> Ptr;
    typedef std::string Id;
    const int chunkSize;
    const int chunkBitShift;

    Map(int chunkSize): chunkSize(chunkSize), chunkBitShift(chunkSize>>1){
    }

    void pregenChunks(int xOffset, int yOffset, int size){
        for(int ix = 0 ; ix < size ; ix++){
            for(int iy = 0 ; iy < size ; iy++) {
                addChunk(
                        loadChunk( (ix + xOffset), (iy + yOffset)),
                        ix + xOffset, iy + yOffset);
            }
        }
    }

    int translateToChunk(coordType dis){
        if(abs(dis) >= 2) {
            return (dis + chunkSize - 1) & -chunkSize;
        }
        return 0;
    }

    /**
     * Will return a chunk if possible.
     * @param x
     * @param y
     * @return
     */
    Chunklet::Ptr getChunklet(int x, int y){

        //If it's not loaded, load it.
        if(map[x][y] == NULL){
            //TODO: if it doesn't exist, generate it
            Chunklet::Ptr chunk = loadChunk(x, y);
            if(chunk == NULL) {
                std::cout << "Failed to load chunk " << x << " " << y << std::endl;
            }else{
                chunks.push_back(chunk);
                map[x][y] = chunk;
                return chunk;
            }
        }
        else{
            return map[x][y];
        }
        return NULL;
    }


    /**
     * When a viewport is inserted, based on its location and size, all nodes in that location need to be registered to it.
     * @param viewport
     * @returns
     *  If the viewport insertion failed, true will return, else false for success.
     */
    bool insert(Viewport &viewport){
        viewports.push_back(viewport);
        return false;
    }

    void draw(TestVisualize &testVisualize){
        for(auto i = chunks.begin(); i != chunks.end(); ++i)
            for(int x = 0; x < chunkSize; x++)
                for(int y = 0; y < chunkSize; y++) {
                    Chunklet *cx = (*i).get();
                    testVisualize.drawPoint(x + cx->x_min, y + cx->y_min);
                }
        for(auto i = nodes.begin(); i != nodes.end(); ++i)
            testVisualize.drawPoint(i->get().getXNode().getPos(), i->get().getYNode().getPos(), sf::Color::Magenta);
    }
};

/**
 * Prepares all maps: for now we'll pregen/load everything immediately
 */
class ChunkMapComposer: private boost::noncopyable{
public:
    typedef std::string Result;
    typedef Map::Id Id;
private:
    std::map<std::string, Map::Ptr> maps;

    //Gen/load this map
    Map::Ptr _loadMap(Id id){
        return Map::Ptr(new Map(8));
    }
public:

    ChunkMapComposer(/*Database connection likely to go here*/){
    }

    /**
     * Loads the desired map
     * @param id of the map to load from the data store
     * @return any error messages. "" upon success, "%id is already loaded" if map is already loaded.
     */
    Result loadMap(Id id){
        std::ostringstream result;
        if(maps[id] == NULL)
            maps[id] = _loadMap(id);
        if(maps[id] == NULL)
            result << "Failure: Load map [" << id << "]. "; //Add additional info
        return result.str();
    }
    bool insert(Node &node){
        if(maps[node.getMapId()] == NULL) {
            Result r = loadMap(node.getMapId());
            if(r == ""){
                if(maps[node.getMapId()]->insert(node)){
                    //Map didn't want the node
                    return true;
                }
                else {
                    //success!
                    node.map = maps[node.getMapId()];
                    return false;
                }
            }
            std::cout << r << std::endl;
        }
        return true;
    }

    Map::Ptr getMap(Id id){
        if(maps[id] == NULL) {
            Result r = loadMap(id);
            if(r == "")
                return maps[id];
        }
        return 0;
    }
    void draw(TestVisualize &testVisualize){
        if(maps.size() > 0)
            maps.begin()->second->draw(testVisualize);
    }
    ~ChunkMapComposer(){
    //    for (std::map<std::string, Map*>::iterator i = maps.begin(); i != maps.end(); ++i)
    //        delete i->second;
    }
};


/**
 * High-level chunk generator-should be able to generate a single tile
 */
class ChunkTileComposer{

};

        } //namespace chunk
    } //namespace vsm
} //namespace vae

#endif //BOOSTTESTING_CHUNKMAP_H
