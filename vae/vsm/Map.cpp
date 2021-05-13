//
// Created by protocol on 4/18/21.
//

#include "Map.h"

vae::vsm::chunk::Chunklet::Chunklet(Map &parent, int chunkSize, int x, int y):
        strand(parent.getComposer().getIoContext()),
        parent(parent),
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
vae::vsm::chunk::Chunklet::~Chunklet(){
    for(int a = 0; a < parent.chunkSize; ++a)
        delete tiles[a];
    delete tiles;
    LOG(Debug) << "Close Chunklet " << x_min << " " << y_min << ".";
}
void vae::vsm::chunk::Chunklet::draw(TestVisualize &testVisualize){
    RLOCK
    //for(auto i = viewports.begin(); i != viewports.end(); ++i)
    //    testVisualize.drawRect(x_min + 4, y_min + 4, 3, 3, sf::Color::Cyan);
if(viewports.size() > 0)
    for(auto i : nodes)
        testVisualize.drawPoint( i->getX(), i->getY(), sf::Color::White);
}
/*
bool vae::vsm::chunk::Chunklet::insert(Node* node){
    //TODO: Ensure we're inserting into a valid location and valid for the node type.
    //To do this, we'll need to have a bunch of high-level logic in place.
    {
        RLOCK
        if (node->getChunklet() != NULL)
            node->getChunklet()->remove(node);
    }

    WLOCK
    nodes.push_back(node);
    //LOG(Debug) << "Inserted node.";
    //TODO: Inform all viewports of the new node (if applicable)

    return false;
}*/

void vae::vsm::chunk::Viewpoint::setY(coordType to){
    if(to < 0)
        to = 0;
    int diff = to - y;
    int h_index = -9999999;
    int map_index = -9999999;

    if(diff > 0){
        for(; y < to; ++y) {
            h_index = y % height;   //this is where in the grid we'll be overwriting:
            map_index = y + height;
            for (int x_index = 0; x_index < width; ++x_index) {
                int w_index = (x_index + x) % width;
                chunkGrid[w_index][h_index]->remove(this);
                chunkGrid[w_index][h_index] = map->getChunklet(x_index + x, map_index);
                chunkGrid[w_index][h_index]->insert(this);
            }
        }
    }
    else if(diff < 0){
        for(; y > to; --y) {
            h_index = (y - 1) % height;
            map_index = (y - 1);
            for (int x_index = 0; x_index < width; ++x_index) {
                int w_index = (x_index + x) % width;
                chunkGrid[w_index][h_index]->remove(this);
                chunkGrid[w_index][h_index] = map->getChunklet(x_index + x, map_index);
                chunkGrid[w_index][h_index]->insert(this);
            }
        }
    }

    //std::cout << "to: " << to << "\t\t diff: " << diff << "\t\t h_index: " << h_index << "\t\t map_index: " << map_index << std::endl;
    y = to;
    //assignChunkGrid();
    return;
}
void vae::vsm::chunk::Viewpoint::setX(coordType to){
    if(to < 0)
        to = 0;
    int diff = to - x;
    int w_index = -9999999;
    int map_index = -9999999;

    if(diff > 0){
        for(; x < to; ++x) {
            w_index = x % width;   //this is where in the grid we'll be overwriting:
            map_index = x + height;
            for (int y_index = 0; y_index < height; ++y_index) {
                int h_index = (y_index + y) % height;
                chunkGrid[w_index][h_index]->remove(this);
                chunkGrid[w_index][h_index] = map->getChunklet(map_index, y_index + y);
                chunkGrid[w_index][h_index]->insert(this);
            }
        }
    }
    else if(diff < 0){
        for(; x > to; --x) {
            w_index = (x - 1) % width;
            map_index = (x - 1);
            for (int y_index = 0; y_index < height; ++y_index) {
                int h_index = (y_index + y) % height;
                chunkGrid[w_index][h_index]->remove(this);
                chunkGrid[w_index][h_index] = map->getChunklet(map_index, y_index + y);
                chunkGrid[w_index][h_index]->insert(this);
            }
        }
    }

    //std::cout << "to: " << to << "\t\t diff: " << diff << "\t\t w_index: " << w_index << "\t\t map_index: " << map_index << std::endl;
    x = to;
    return;
}

void vae::vsm::chunk::Viewpoint::assignChunkGrid(){
    for(int a = 0; a < width; a++)
        for(int b = 0; b < height; b++) {
            int xi = (a + v_index) % width;
            int yi = (b + h_index) % height;
            int gridX = x + a;
            int gridY = y + b;
            int x_index = (a + x) % width;
            int y_index = (b + y) % height;
            if(chunkGrid[x_index][y_index] != nullptr)
                chunkGrid[x_index][y_index]->remove(this);
            chunkGrid[x_index][y_index] = map->getChunklet(gridX, gridY);
            chunkGrid[x_index][y_index]->insert(this);
        }
}
void vae::vsm::chunk::Viewpoint::draw(TestVisualize &testVisualize){
    for(int a = 0; a < width; a++)
        for(int b = 0; b < height; b++)
            if(chunkGrid[a][b] != NULL)
                testVisualize.drawRect(chunkGrid[a][b]->x_min + 1, chunkGrid[a][b]->y_min + 1, map->chunkSize - 3, map->chunkSize - 3, sf::Color::Blue);
}
/*
bool vae::vsm::chunk::Node::setX(coordType to){
    Chunklet::Ptr targetChunk = getMap()->getChunklet( getMap()->translateToChunk(to), getChunklet()->chunk_y);
    if( targetChunk.get() != getChunklet().get() ){
        if(targetChunk->insert(this)) {
            LOG(Warn) << "Insert failure.";
            return true;
        }
        else
            this->chunk = targetChunk;
    }
    pos.x = to;
    return false;
}
bool vae::vsm::chunk::Node::setY(coordType to){
    Chunklet::Ptr targetChunk = getMap()->getChunklet(getChunklet()->chunk_x, getMap()->translateToChunk(to));
    if( targetChunk.get() != getChunklet().get() ){
        if(targetChunk->insert(this)) {
            LOG(Warn) << "Insert failure.";
            return true;
        }
        else
            this->chunk = targetChunk;
    }
    pos.y = to;
    return false;
}*/
bool vae::vsm::chunk::Node::setPos(coordType x, coordType y){
    map->setNodePos(this, getX(), getY(), boost::bind(&vae::vsm::chunk::Node::_setPos, this, _1, _2, _3), chunk, x, y);



    /*
    Chunklet::Ptr targetChunk = getMap()->getChunklet(getMap()->translateToChunk(x), getMap()->translateToChunk(y));
    if(targetChunk.get() != getChunklet().get()){
        targetChunk->take(this, boost::bind(&vae::vsm::chunk::Node::_setPos, this), x, y);

        //work_io_service_.post (boost::bind(&LogService::log_impl, this, system, time, level, lv, file, line, function, message));

        if (targetChunk->insert(this)) {
            LOG(Warn) << "Insert failure.";
            return true;
        }
        this->chunk = targetChunk;
    }
    pos.x = x;
    pos.y = y;*/
    return false;
}

bool vae::vsm::chunk::Node::_setPos(Chunklet::Ptr targetChunk, coordType x, coordType y){
    this->chunk = targetChunk;
    this->pos.x = x;
    this->pos.y = y;
}

//dist = speed * delta_t
double vae::vsm::chunk::Node::movePos(VectorDirectionType vd, SpeedType dist){
    SpeedType maxSpeed = vae::vsm::chunk::Map::maxSpeed;
    while(dist > 0){
        SpeedType move = (dist > maxSpeed) ? maxSpeed : dist;
        vae::vsm::chunk::coordType newX = pos.x + (cos(vd) * dist);
        vae::vsm::chunk::coordType newY = pos.y + (sin(vd) * dist);
        if(setPos(newX, newY)){
            //couldn't move, return the remaining dist we didn't move
            return dist;
        }
        dist -= move;
    }
    return false;
}
bool vae::vsm::chunk::NodeList::setPos(coordType to)
{
/*    Chunklet::Ptr targetChunk;
    if (to > node.getChunklet()->x_max) {
        //TODO: Ensure we're moving within bounds of the chunkMap
        targetChunk = node.getMap()->getChunklet(node.getChunklet()->chunk_x + 1, node.getChunklet()->chunk_y);
    }
    else if (to < node.getChunklet()->x_min) {
        targetChunk = node.getMap()->getChunklet(node.getChunklet()->chunk_x - 1, node.getChunklet()->chunk_y);
    }
    else if (to > node.getChunklet()->y_max) {
        targetChunk = node.getMap()->getChunklet(node.getChunklet()->chunk_x, node.getChunklet()->chunk_y + 1);
    }
    else if (to < node.getChunklet()->y_min) {
        targetChunk = node.getMap()->getChunklet(node.getChunklet()->chunk_x, node.getChunklet()->chunk_y - 1);
    }
    else
        targetChunk = node.getChunklet();

    if(targetChunk->insert(&node)){
        std::cout << "Failed to move node." << std::endl;
        return true;
    }
*/
    this->pos = to;
    return false;
}
