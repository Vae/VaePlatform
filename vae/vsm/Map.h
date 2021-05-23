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
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <vae/Datastore.h>


#include "TestVisualize.h"
#include "vae/react/Action.h"
#include "olib/concurrentqueue.h"
#include "vae/log.h"

#include "../Service.h"

/**
 *
 * Vaewyn Server Mapping
 *
 * The high level overview:
 *   Each map can be large, only positive coordinates and contain any number of entities.
 *   A map is comprised of several layers:
 *     * Tile
 *     * Collision
 *
 *
 * The server map contsists of a grid of Chunklets that comprise the map.
 *
 */

#define FIRST_MAP "chunkMap-zero"

namespace vae::vsm::chunk {

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
    typedef double BaseUnitType;
    typedef double SpeedType;
    typedef double LenType;
    typedef int DimType;
    typedef int ChunkCoord;
    typedef double AngleType;
    typedef AngleType VdType;
    typedef double RadiusType;

    struct Vector {
        coordType x, y;
        Vector(coordType x, coordType y): x(x), y(y){}
        static BaseUnitType getLength(const Vector & dis){
            return dis.x * dis.x + dis.y * dis.y;
        }
        static BaseUnitType getAngle(const Vector & dis){
            return dis.y > 0.0f ? acos(dis.x / getLength(dis)) : -acos(dis.x / getLength(dis));
        }
    };

    typedef std::string MapId;

    class Tile: private boost::noncopyable{
        //placeholder
        int tileNum;
    };

    class NodeInterface{
        const Map &map;
    public:
        //Returns the distance traveled
        virtual LenType move(VdType vd, SpeedType speed)=0;
        virtual void setPos(coordType x, coordType y)=0;
        virtual void setSize(RadiusType r)=0;
    };

    class ViewportInterface{
        const Map &map;
    public:
        typedef unsigned int DimType;
        virtual void subscribe(NodeInterface *node)=0;
        virtual void unsubscribe(NodeInterface *node)=0;
    };

    class MapInterface{
    public:
        //virtual NodeInterface

        virtual void insert(ViewportInterface *vp)=0;
        virtual void remove(ViewportInterface *vp)=0;
        virtual void move(ViewportInterface *vp)=0;

        virtual void insert(Node *node)=0;
        virtual void remove(Node *node)=0;
    };

    class NodeAppearance{
        sf::Color color;
    public:
        typedef std::shared_ptr<NodeAppearance> Ptr;
        NodeAppearance(sf::Color c): color(c){
        }
    };

    class NodeAction /*: private boost::noncopyable*/{
    public:
        typedef std::shared_ptr<NodeAction> Ptr;
        const std::shared_ptr<Node> owner;
        NodeAction(std::shared_ptr<Node> owner): owner(owner){
        }
    };

    class NodeActionConsumer{
    public:
        virtual void witness(size_t typeHash, NodeAction::Ptr dis)=0;
    };

    class ViewportGrip: private boost::noncopyable{
        virtual void changeNode(size_t typeHash, NodeAction::Ptr dis)=0;    //A node did something

        virtual boost::asio::io_context::strand& getStrand()=0;
        virtual void updatePos(coordType x, coordType y, DimType width, DimType height)=0;
        virtual void updateMapId(MapId id)=0;
    public:
        void postUpdatePos(coordType x, coordType y, DimType width, DimType height){
            getStrand().post(boost::bind(&ViewportGrip::updatePos, this, x, y, width, height));
        }
        void postUpdateMapId(MapId id){
            getStrand().post(boost::bind(&ViewportGrip::updateMapId, this, id));
        }
        void postChangeNode(size_t typeHash, NodeAction::Ptr dis){
            getStrand().post(boost::bind(&ViewportGrip::changeNode, this, typeHash, dis));
        }
    };

//Consumer
    class Viewpoint: private boost::noncopyable{
        friend class Map;
        //May want to change this to x1, y1, x2, y2
        DimType width, height;

        //DimType w, h;
        ChunkCoord x, y;

        std::vector<std::vector<std::shared_ptr<Chunklet>>> chunkGrid;
        int h_index = 0;
        int v_index = 0;
        ViewportGrip &grip;

        Map *parentMap;

        void assignChunkGrid();
    public:
        ViewportGrip &getGrip() { return grip; }
        //moodycamel::ConcurrentQueue<std::shared_ptr<vae::react::Action>> action_queue;
        typedef std::shared_ptr<Viewpoint> Ptr;
        Viewpoint(ViewportGrip &viewportGrip, Map *parentMap, coordType x, coordType y, DimType width, DimType height);
        virtual ~Viewpoint(){
            LOG(Info) << "Close Viewport.";
        }
        //int getWidth() const { return width; }
        //void setWidth(int width) {Viewport::width = width;}
        //int getHeight() const { return height; }
        //void setHeight(int height) {Viewport::height = height;}
        //coordType getX() const { return x; }
        void setX(coordType x);
        void setY(coordType y);
        //coordType getY() const { return y; }
        Map *getMap() const { return parentMap; }
    };

    class Chunklet: private boost::noncopyable, public NodeActionConsumer{
        Tile **tiles;
        Lock lock;
        Map &parent;

        /**
         * Since I want to try and have each Chunklet be on its own thread, they'll have to maintain their own lists.
         */
        std::list<std::shared_ptr<Node>> nodes;
        std::list<Viewpoint*> viewports;

        //moodycamel::ConcurrentQueue<vae::react::Action::Ptr> action_queue;
    public:
        typedef std::shared_ptr<Chunklet> Ptr;
        const int chunk_x;
        const int chunk_y;
        const int x_min;
        const int y_min;
        const int x_max;
        const int y_max;

        //Strand will be used to push viewport events onto
        //  Which will then pass that info to interfaces: interfaces do not directly interact with what vp sees
        //boost::asio::io_context::strand strand;

        Chunklet(Map &parent, int chunkSize, int x, int y);
        virtual ~Chunklet();

        void witness(size_t typeHash, NodeAction::Ptr dis){
            for(auto i : viewports)
                i->getGrip().postChangeNode(typeHash, dis);
        }

        //void ReadFunction(){
        //    ReadLock r_lock(lock);
        //    //Do reader stuff
        //}
        //void WriteFunction(){
        //    WriteLock w_lock(lock);
        //    //Do writer stuff
        //}
        void insert(vae::react::Action<Node, Viewpoint>::Ptr action){
            //       for(auto i : viewports)
//            i->action_queue.enqueue(action);
        }

        bool insert(Viewpoint *viewport);
        void remove(Viewpoint *viewport);
        void draw(TestVisualize &testVisualize);
        bool insert(std::shared_ptr<Node> node){
            nodes.push_back(node);
        }
        void remove(std::shared_ptr<Node> node){
            nodes.remove(node);
        }

//        void exchange(Node *node, coordType x, coordType y, boost::function<void(Chunklet::Ptr targetChunk, coordType x, coordType y)> cb_completed, Chunklet::Ptr newChunk){
//            //We need to give newChunk the node
//            //Step 1: Call _yield to get ride of our node and call newChunklet when we're done
//            //strand.post(boost::bind<void>(&Chunklet::_exchange, this, node, x, y, cb_completed, newChunk));
//            nodes.remove(node);
//            newChunk->take(node, x, y, cb_completed, newChunk);
//        }
//        void take(Node *node, coordType x, coordType y, boost::function<void(Chunklet::Ptr targetChunk, coordType x, coordType y)> cb_completed, Chunklet::Ptr newChunk){
//            //newChunk will be a smart pointer to myself (this)
//            strand.post(boost::bind<void>(&Chunklet::_take, this, node, x, y, cb_completed, newChunk));
//        }
//        void yield(Node *node){
//            strand.post(boost::bind<void>(&Chunklet::_yield, this, node));
//        }
    private:
//       //As always, _ underscore methods are to be only called via callback.
//       void _take(Node *node, coordType x, coordType y, boost::function<void(Chunklet::Ptr targetChunk, coordType x, coordType y)> cb_completed, Chunklet::Ptr newChunk){
//           //newChunk will be a smart pointer to myself (this)
//           nodes.push_back(node);
//           cb_completed(newChunk, x, y);
//       }
//       void _exchange(Node *node, coordType x, coordType y, boost::function<void(Chunklet::Ptr targetChunk, coordType x, coordType y)> cb_completed, Chunklet::Ptr newChunk){
//           nodes.remove(node);
//           //newChunk->strand.post(cb_completed(newChunk, x, y));
//           //newChunk->strand.post(boost::bind<void>(node, x, y, cb_completed, newChunk));
//           newChunk->take(node, x, y, cb_completed, newChunk);
//       }
//       void _yield(Node *node){
//           nodes.remove(node);
//       }
    };

    class NodeGrip: private boost::noncopyable{
        virtual boost::asio::io_context::strand& getStrand()=0;
        virtual void updatePos(Vector pos)=0;
        virtual void updateMapId(MapId id)=0;
        virtual boost::signals2::signal<void ()> &get(){
            boost::signals2::signal<void ()> sig;
        };
    public:
        void postUpdatePos(Vector pos){
            getStrand().post(boost::bind(&NodeGrip::updatePos, this, pos));
        }
        void postUpdateMapId(MapId id){
            getStrand().post(boost::bind(&NodeGrip::updateMapId, this, id));
        }
    };

//Producer
    class Node: private boost::noncopyable, public std::enable_shared_from_this<Node> {
        friend class Composer;
        friend class Map;
        friend class Chunklet;
        Vector pos;
        Chunklet::Ptr chunk;
        Map &map;
        NodeGrip &grip;
        NodeAppearance::Ptr appearance;
        const MapId mapId;
        //Issue: an Entity, while about to cross a Chunklet, will briefly be in two Chunklets at the same time
        //or, if a entity is large enough, it could encompass multiple chunklets
        std::map<coordType, std::map<coordType, Chunklet::Ptr>> proxyChunks;
    public:
        boost::signals2::signal<void (NodeAction &nodeAction)> observers;
        typedef std::shared_ptr<Node> Ptr;
        ~Node();

        /*
        //https://embeddedartistry.com/blog/2017/01/11/stdshared_ptr-and-shared_from_this/
        template<typename ... T>
        static std::shared_ptr<Node> create(T&& ... t) {
            //return std::shared_ptr<Node>(new Node(std::forward<T>(t)...));
            return std::make_shared<vae::vsm::chunk::Node>(std::forward<T>(t)...);
        }
        */
        void doAction(size_t typeHash, NodeAction::Ptr dis);
        void setAppearance(NodeAppearance::Ptr appearance);
        bool setPos(coordType x, coordType y);
        void movePos(VectorDirectionType vd, SpeedType dist);     //dist = speed * delta_t
        MapId getMapId() const { return mapId; }
    private:
        void _setAppearance(NodeAppearance::Ptr appearance);
        void _doAction(size_t typeHash, NodeAction::Ptr dis);
        void _movePos(VectorDirectionType vd, SpeedType dist);     //dist = speed * delta_t
        Node(Map &map, NodeGrip &grip, coordType x, coordType y);
        bool _setPos(Chunklet::Ptr targetChunk, coordType x, coordType y);
    };

/**
* Holds Nodes
*/
class Map: private boost::noncopyable {
        friend class Composer;
        friend class Node;
    private:
        boost::asio::io_context &ioContext;
        Lock lock;
        Composer& composer;
        int addChunk(Chunklet::Ptr dis, coordType x, coordType y){
            chunks.push_back(dis);
            chunkMap[x].emplace(y, dis);
            return 0;
        }
        std::map<coordType, std::map<coordType, Chunklet::Ptr>> chunkMap;
        std::list<Chunklet::Ptr> chunks;
        std::list<Node::Ptr> nodes;
        //boost::numeric::ublas::mapped_matrix<int> nodeMap;
        std::list<Viewpoint*> viewports;
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
        bool insert(Viewpoint &viewport){
            viewports.push_back(&viewport);
            return false;
        }

        void a_newNode(coordType x, coordType y, boost::asio::io_context::strand &strand, boost::function<void>(Node *node, coordType x, coordType y)){
        }
        void removeNode(Node::Ptr node){
            nodes.remove(node);
        }

    Map(boost::asio::io_context &ioContext, Composer &composer, MapId myId, int chunkSize):
                ioContext(ioContext),
                strand(ioContext),
                composer(composer),
                id(myId),
                chunkSize(chunkSize)/*, nodeMap(1048575, 1048575)*/{
            LOG(Info) << "Map loaded " << id << "<" << this << ">.";
        }
        void _newNode(Node::Ptr node, Chunklet::Ptr dest){
            nodes.push_back(node);
            dest->insert(node);
            node->grip.postUpdateMapId(this->id);
        }
        void _newViewport(Viewpoint *vp){
            viewports.push_back(vp);
            vp->assignChunkGrid();
            vp->getGrip().postUpdateMapId(this->id);
        }
    public:
        boost::asio::io_context::strand strand;
        typedef std::shared_ptr<Map> Ptr;
        const int chunkSize;
        constexpr const static SpeedType maxSpeed = 1;

        const MapId id;
        const MapId getId() const { return id; }
        Composer &getComposer() { return composer; }

        inline Chunklet::Ptr getChunkTranslate(coordType x, coordType y){
            //TODO: Ensure the node is in bounds defined by the Map shape.
            return getChunklet(translateToChunk(x), translateToChunk(y));
        }

        /**
         * When a node is insert, based on its location and size, it needs to be added to all consumers of that location.
         * @param node
         * @returns bool
         *  If the node insertion failed, true will return, else false for success.
         */
        Node::Ptr newNode(NodeGrip &nodeGrip, coordType x, coordType y){
            Chunklet::Ptr dest = getChunkTranslate(x, y);
            if(dest == NULL) {
                LOG(Warn) << "Failed to insert node: out of bounds.";
                return nullptr;
            }
            Node::Ptr node(new Node(*this, nodeGrip, x, y));
            strand.post(boost::bind(&Map::_newNode, this, node, dest));

    //void take(Node *node, coordType x, coordType y, boost::function<void(Chunklet::Ptr targetChunk, coordType x, coordType y)> cb_completed, Chunklet::Ptr newChunk){
            //dest->take(&node, x, y, boost::bind<void>(&Node::_setPos, &node, dest, x, y), dest);

            return node;
        }

        Viewpoint* newViewpoint(ViewportGrip &viewportGrip, coordType x, coordType y, DimType width, DimType height){
            Viewpoint* vp = new Viewpoint(viewportGrip, this, x, y, width, height);
            strand.post(boost::bind(&Map::_newViewport, this, vp));
            return vp;
        }

        virtual ~Map(){
            LOG(Info) << "Close Map.";
        }

        void pregenChunks(int xOffset, int yOffset, int size){
//            WLOCK
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
//            WLOCK
            x = translateToChunk(x);
            y = translateToChunk(y);
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

        //void setNodePos(Node *node, coordType targetX, coordType targetY, boost::function<void(LenType dist, coordType x, coordType y)> cb_completed, boost::asio::io_context::strand &postTo){
        //}
//
        //void _setNodePos(Node *node, coordType targetX, coordType targetY, boost::bind(&NodeController::cycle, this) cb_completed, boost::asio::io_context::strand &postTo){
        //    Chunklet::Ptr tChunk = getChunklet(translateToChunk(targetX), translateToChunk(targetY));
        //    if(tChunk.get() == node->chunk.get())
        //        postTo.post(cb_completed(targetX, targetY));
        //}

//        void setNodePos(Node *node, coordType x, coordType y, boost::function<void(Chunklet::Ptr targetChunk, coordType x, coordType y)> cb_completed, Chunklet::Ptr currentChunk, coordType targetX, coordType targetY){
//            Chunklet::Ptr targetChunk = getChunklet(translateToChunk(x), translateToChunk(y));
//            if(targetChunk.get() == currentChunk.get())
//                cb_completed(targetChunk, targetX, targetY);
//            else
//                strand.post(boost::bind<void>(&Map::_exchange, this, node, targetChunk));
//            ioContext.post(cb_completed);
//        }
        void _exchange(Node::Ptr node, Chunklet::Ptr to){
            node->chunk->remove(node);
            to->insert(node);
            node->chunk = to;
        }
        void _insert(Node::Ptr node, Chunklet::Ptr chunk){
            chunk->insert(node);
        }
        void _remove(Node::Ptr node, Chunklet::Ptr chunk){
            chunk->remove(node);
        }
        void draw(TestVisualize &testVisualize){
            for(auto i = chunks.begin(); i != chunks.end(); ++i){
                Chunklet *cx = (*i).get();
            //    testVisualize.drawRect(cx->x_min, cx->y_min, chunkSize, chunkSize, sf::Color(255, 255, 255, 30));
                cx->draw(testVisualize);
            }
            //for(auto i = nodes.begin(); i != nodes.end(); ++i)
            //    testVisualize.drawPoint(i->get().getX(), i->get().getY(), sf::Color::Magenta);
            //for(auto i = viewports.begin(); i != viewports.end(); ++i) {
            //    Viewpoint *cx = *i;
            //    testVisualize.drawRect(cx->getX() * chunkSize, cx->getY() * chunkSize, cx->getWidth() * chunkSize, cx->getHeight() * chunkSize, sf::Color::Green);
            //}
        }
    };

/**
* Prepares all maps: for now we'll pregen/load everything immediately
*/
    class Composer: public vae::Service{
    public:
        typedef std::string Result;
    private:
        boost::asio::io_context &ioContext;
        vae::Datastore::Ptr ds;
        std::map<std::string, Map::Ptr> maps;

        //Gen/load this chunkMap
        Map::Ptr _loadMap(MapId id){
            return Map::Ptr(new Map(ioContext, *this, id, 8));
        }
/**
* Loads the desired chunkMap
* @param id of the chunkMap to load from the data store
* @return any error messages. "" upon success, "%id is already loaded" if chunkMap is already loaded.
*/
        Result loadMap(MapId id){
            LOG(Info) << "Load map [" << id << "]";
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
            //LOG(Debug) << "Ready map [" << id << "]";
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
    public:
        boost::asio::io_context &getIoContext() { return ioContext; }
        ~Composer(){
            LOG(Info) << "Close MapComposer.";
        }
        Composer(boost::asio::io_context &ioContext, vae::Datastore::Ptr ds): Service("MapComposer"), ioContext(ioContext), ds(ds){
        }
        //Node* newNode(MapId mapId, coordType x, coordType y){
        //    //inserting onto a valid map?
        //    if(mapId  == "")
        //        return nullptr;
        //    //inserting onto a load-able map?
        //    if(readyMap(mapId))
        //        return nullptr;
        //    return maps[mapId]->newNode(x, y);
        //}
        Map::Ptr getMap(MapId mapId){
            if(mapId  == "")
               return nullptr;
            //inserting onto a load-able map?
            if(readyMap(mapId))
               return nullptr;
            return maps[mapId];
        }



//        Map::Ptr getMap(MapId id){
//            if(maps[id] == NULL) {
//                Result r = loadMap(id);
//                if(r == "")
//                    return maps[id];
//            }
//            return 0;
//        }
        //void draw(TestVisualize &testVisualize){
        //    if(maps.size() > 0)
        //        maps.begin()->second->draw(testVisualize);
        //}
    };
} //namespace vae::vsm::chunk


#endif //BOOSTTESTING_MAP_H
