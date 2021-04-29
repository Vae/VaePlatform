//
// Created by protocol on 4/18/21.
//

#ifndef BOOSTTESTING_MAP_H
#define BOOSTTESTING_MAP_H
#include <vector>
#include <list>
#include <shared_mutex>

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/noncopyable.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>

#include "TestVisualize.h"
#include "../react/Action.h"
#include "../../olib/concurrentqueue.h"
#include "../../vae/log.h"

namespace vae {
    namespace vsm {
        namespace chunk {
            class Map;
            class Tile;
            class Node;
            class Chunklet;
            class Composer;

            typedef std::shared_mutex Lock;
            typedef std::unique_lock< Lock > WriteLock;
            typedef std::shared_lock< Lock > ReadLock;

#define RLOCK ReadLock r_lock(lock);
#define WLOCK WriteLock w_lock(lock);

            typedef double coordType;
            typedef double VectorDirection;
            struct Vector {
                coordType x, y;
                Vector(coordType x, coordType y): x(x), y(y){}
                static float getLength(const Vector & dis){
                    return dis.x * dis.x + dis.y * dis.y;
                }
                static float getAngle(const Vector & dis){
                    return dis.y > 0.0f ? acos(dis.x / getLength(dis)) : -acos(dis.x / getLength(dis));
                }
            };

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
                int x, y;

                std::vector<std::vector<std::shared_ptr<Chunklet>>> chunkGrid;
                int h_index = 0;
                int v_index = 0;

                std::shared_ptr<Map> map;
                MapId mapId;
                void assignChunkGrid();
            public:
                //moodycamel::ConcurrentQueue<std::shared_ptr<vae::react::Action>> action_queue;
                typedef std::shared_ptr<Viewport> Ptr;
                Viewport(int width, int height): width(width), height(height), x(0), y(0), chunkGrid(width, std::vector<std::shared_ptr<Chunklet>>(height)) {
                }
                Viewport(int x, int y, int width, int height): width(width), height(height), x(x), y(y), chunkGrid(width, std::vector<std::shared_ptr<Chunklet>>(height)) {
                }
                virtual ~Viewport(){
                    LOG(Info) << "Close Viewport.";
                }

                void cycle(float timestamp){
                //        while(action_queue.size_approx() > 0){
                //            //TODO: do observations
                //        }
                }

                void draw(TestVisualize &testVisualize);

                int getWidth() const { return width; }
                //void setWidth(int width) {Viewport::width = width;}
                int getHeight() const { return height; }
                //void setHeight(int height) {Viewport::height = height;}
                coordType getX() const { return x; }
                void setX(coordType x);
                coordType getY() const { return y; }
                void setY(coordType y);
                std::shared_ptr<Map> getMap() const { return map; }
                void setMap(std::shared_ptr<Map> to) {
                    map = to;
                    for(int a = 0; a < width; ++a)
                        chunkGrid[a].clear();
                    assignChunkGrid();
                }
            };

            class Chunklet: private boost::noncopyable{
                Tile **tiles;
                Lock lock;
                Map &parent;

                //Strand will be used to push viewport events onto
                //  Which will then pass that info to interfaces: interfaces to not directly interact with what vp sees
                //boost::asio::io_context::strand strand;

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

                Chunklet(Map &parent, int chunkSize, int x, int y);
                virtual ~Chunklet();

                //void ReadFunction(){
                //    ReadLock r_lock(lock);
                //    //Do reader stuff
                //}
                //void WriteFunction(){
                //    WriteLock w_lock(lock);
                //    //Do writer stuff
                //}
                void insert(vae::react::Action<Node>::Ptr action){
                    //       for(auto i : viewports)
//            i->action_queue.enqueue(action);
                }

                bool insert(Viewport *viewport){
                    viewports.push_back(viewport);
                    return false;
                }
                void remove(Viewport *viewport){
                    viewports.remove(viewport);
                }
                void draw(TestVisualize &testVisualize){
                    RLOCK
                    for(auto i = viewports.begin(); i != viewports.end(); ++i)
                        testVisualize.drawRect(x_min + 4, y_min + 4, 3, 3, sf::Color::Cyan);
                }
                bool insert(Node *node);
                void remove(Node *node){
                    WLOCK
                    //TODO: Inform viewports that the node left
                    nodes.remove(node);
                    //std::cout << "Removed node." << std::endl;
                }
            };

//Producer
            class Node: private boost::noncopyable {
                friend class Composer;
                friend class Map;
                Vector pos;
                Chunklet::Ptr chunk;
                std::shared_ptr<Map> map;
                MapId mapId;
            public:
                typedef std::shared_ptr<Node> Ptr;
                Node(): pos(0, 0){
                    LOG(Debug) << "New Node <" << this << ">.";
                }
                ~Node(){
                    LOG(Debug) << "Lost Node <" << this << ">.";
                    //Unregister chunkMap
                }
                /*vae::react::Action<Node>::Ptr say(std::string dis){
                    auto act = [&](Node::Ptr tgt){
                    };
                    return new vae::react::Action<Node>(this, act);
                }*/
                /*
                //https://embeddedartistry.com/blog/2017/01/11/stdshared_ptr-and-shared_from_this/
                template<typename ... T>
                static std::shared_ptr<Node> create(T&& ... t) {
                    //return std::shared_ptr<Node>(new Node(std::forward<T>(t)...));
                    return std::make_shared<vae::vsm::chunk::Node>(std::forward<T>(t)...);
                }
                */


                void setMap(std::shared_ptr<Map> to){ this->map = to; }

                bool setX(coordType to);
                bool setY(coordType to);
                bool setPos(coordType x, coordType y);

                //dist = speed * delta_t
                double movePos(VectorDirectionType vd, SpeedType dist);

                coordType getX() { return pos.x; }
                coordType getY() { return pos.y; }
                Chunklet::Ptr getChunklet() const { return chunk; }
                std::shared_ptr<Map> getMap() const { return map; }
                MapId getMapId() const { return mapId; }
                void setMapId(MapId to) {
                    //TODO: Some sort of check here, should not be able to set mapId without keeping chunkMap in sync
                    mapId = to;
                    if(map.get() != NULL)
                        LOG(Warn) << "Set chunkMap ID with a valid chunkMap already set!";
                }
                void setChunklet(Chunklet::Ptr to) { chunk = to; }
            };

/**
 * Holds Nodes
 */
            class Map: private boost::noncopyable {
                friend class Composer;
            private:
                Lock lock;
                Composer& composer;
                int addChunk(Chunklet::Ptr dis, coordType x, coordType y){
                    chunks.push_back(dis);
                    chunkMap[x].emplace(y, dis);
                    return 0;
                }
                std::map<coordType, std::map<coordType, Chunklet::Ptr>> chunkMap;
                std::list<Chunklet::Ptr> chunks;
                std::list<std::reference_wrapper<Node>> nodes;
                //boost::numeric::ublas::mapped_matrix<int> nodeMap;
                std::list<std::reference_wrapper<Viewport>> viewports;
                /**
                 * @param x
                 * @param y
                 * @return pointer to the loaded chunk
                 */
                Chunklet::Ptr loadChunk(int x, int y){
                    //std::cout << "New chunk " << x << " " << y << std::endl;
                    return Chunklet::Ptr(new Chunklet(*this, chunkSize, x, y));
                    //return std::make_shared(*this, chunkSize, x, y);
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

                /**
                 * When a node is insert, based on its location and size, it needs to be added to all consumers of that location.
                 * @param node
                 * @returns bool
                 *  If the node insertion failed, true will return, else false for success.
                 */
                bool insert(Node &node, coordType x, coordType y){
                    //TODO: Ensure the node is in bounds defined by the Map shape
                    //Ensure the chunk is loaded; if not, load it.
                    int chunk_x = translateToChunk(x);
                    int chunk_y = translateToChunk(y);

                    Chunklet::Ptr dest = getChunklet(chunk_x, chunk_y);

                    if(dest == NULL || dest->insert(&node)) {
                        LOG(Warn) << "Failed to insert node.";
                        return true;
                    }
                    node.pos.x = x;
                    node.pos.y = y;
                    node.setChunklet(dest);
                    nodes.push_back(node);
                    return false;
                }
                //Will get a rectangle lock
                void grabChunkLock(int x, int y){
                }
                Map(Composer &composer, MapId myId, int chunkSize): composer(composer), id(myId), chunkSize(chunkSize), chunkBitShift(chunkSize>>1)/*, nodeMap(1048575, 1048575)*/{
                    LOG(Info) << "Map loaded " << id << "<" << this << ">.";
                }
            public:
                typedef std::shared_ptr<Map> Ptr;
                typedef std::string Id;
                const int chunkSize;
                const int chunkBitShift;
                constexpr const static SpeedType maxSpeed = 1;

                const MapId id;

                virtual ~Map(){
                    LOG(Info) << "Close Map.";
                }

                Composer& getComposer() { return composer; }

                void pregenChunks(int xOffset, int yOffset, int size){
                    WLOCK
                    for(int ix = 0 ; ix < size ; ix++){
                        for(int iy = 0 ; iy < size ; iy++) {
                            addChunk(
                                    loadChunk( (ix + xOffset), (iy + yOffset)),
                                    ix + xOffset, iy + yOffset);
                        }
                    }
                }

                int translateToChunk(coordType dis){
                    //return ((dis - 1) / (chunkSize-1)) * (chunkSize-1);
                    return dis / chunkSize;
                }

                /**
                 * Will return a chunk if possible.
                 * @param x
                 * @param y
                 * @return
                 */
                Chunklet::Ptr getChunklet(int x, int y){
                    WLOCK
                    //If it's not loaded, load it.
                    if(chunkMap[x][y] == NULL){
                        //TODO: if it doesn't exist, generate it
                        Chunklet::Ptr chunk = loadChunk(x, y);
                        if(chunk == NULL) {
                            LOG(Warn) << "Failed to load chunk " << x << " " << y;
                        }else{
                            chunks.push_back(chunk);
                            chunkMap[x][y] = chunk;
                            return chunk;
                        }
                    }
                    else{
                        return chunkMap[x][y];
                    }
                    return NULL;
                }

                void draw(TestVisualize &testVisualize){
                    RLOCK
                    //for(auto i = chunks.begin(); i != chunks.end(); ++i){
                    //    Chunklet *cx = (*i).get();
                    //    testVisualize.drawRect(cx->x_min, cx->y_min, chunkSize, chunkSize, sf::Color(255, 255, 255, 30));
                    //    cx->draw(testVisualize);
                    //}
                    for(auto i = nodes.begin(); i != nodes.end(); ++i)
                        testVisualize.drawPoint(i->get().getX(), i->get().getY(), sf::Color::Magenta);
                    for(auto i = viewports.begin(); i != viewports.end(); ++i) {
                        i->get().draw(testVisualize);
                        //testVisualize.drawRect(i->get().getX(), i->get().getY(), i->get().getWidth() * chunkSize, i->get().getHeight() * chunkSize,
                        //                       sf::Color::Green);
                    }
                }
            };

/**
 * Prepares all maps: for now we'll pregen/load everything immediately
 */
            class Composer: private boost::noncopyable{
            public:
                typedef std::string Result;
                typedef Map::Id Id;
            private:
                std::map<std::string, Map::Ptr> maps;
                std::string connectionString;

                //Gen/load this chunkMap
                Map::Ptr _loadMap(Id id){
                    return Map::Ptr(new Map(*this, id, 8));
                }
/**
 * Loads the desired chunkMap
 * @param id of the chunkMap to load from the data store
 * @return any error messages. "" upon success, "%id is already loaded" if chunkMap is already loaded.
 */
                Result loadMap(Id id){
                    std::ostringstream result;
                    if(maps[id] == NULL)
                        maps[id] = _loadMap(id);
                    if(maps[id] == NULL) {
                        result << "Failure: Load chunkMap [" << id << "]. "; //Add additional info
                        LOG(Warn) << result.str();
                    }
                    else
                        maps[id]->pregenChunks(0,0 , 1);

                    return result.str();
                }
                bool readyMap(MapId id){
                    LOG(Info) << "Ready map [" << id << "]";
                    if(maps[id] == NULL) {
                        Result r = loadMap(id);
                        if(r == ""){
                            return false;
                        }
                    }
                    else    //Map is already loaded
                        return false;
                    return true;
                }
                boost::asio::io_context &ioContext;
            public:
                boost::asio::io_context &getIoContext() { return ioContext; }
                ~Composer(){
                    LOG(Info) << "Close MapComposer.";
                }
                Composer(boost::asio::io_context &ioContext, std::string connectionString): ioContext(ioContext), connectionString(connectionString){
                }

                bool insert(Node& node, coordType x, coordType y){
                    //inserting onto a valid map?
                    if(node.getMapId()  == "")
                        return true;
                    //inserting onto a load-able map?
                    if(readyMap(node.getMapId()))
                        return true;
                    //
                    if(maps[node.getMapId()]->insert(node, x, y)){
                        //Map didn't want the node
                        return true;
                    }
                    else {
                        //success!
                        node.map = maps[node.getMapId()];
                        return false;
                    }
                    return true;
                }
                bool insert(Viewport &vp, MapId id){
                    if(readyMap(id))
                        return true;
                    std::shared_ptr<Map> m = maps[id];
                    vp.setMap(m);
                    m->insert(vp);
                    return false;
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
            };
        } //namespace chunk
    } //namespace vsm
} //namespace vae


#endif //BOOSTTESTING_MAP_H
