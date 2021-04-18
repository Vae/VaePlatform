//
// Created by protocol on 4/14/21.
//

#include "ChunkMap.h"


bool vae::vsm::chunk::Chunklet::insert(Node *node){
    //TODO: Ensure we're inserting into a valid location and valid for the node type.
    //To do this, we'll need to have a bunch of high-level logic in place.

    if(node->getChunklet() != NULL)
        node->getChunklet()->remove(node);

    nodes.push_back(node);
    std::cout << "Inserted node." << std::endl;
    //TODO: Inform all viewports of the new node (if applicable)

    return false;
}

void vae::vsm::chunk::Viewport::assignChunkGrid(){
    for(int a = 0; a < width; a++)
        for(int b = 0; b < height; b++) {
            int xi = (b + v_index) % width;
            int yi = (a + h_index) % height;
            int gridX = x + b;
            int gridY = y + a;
            chunkGrid[xi][yi] = map->getChunklet(gridX, gridY);
        }
}
void vae::vsm::chunk::Viewport::draw(TestVisualize &testVisualize){
    for(int a = 0; a < width; a++)
        for(int b = 0; b < height; b++)
            if(chunkGrid[a][b] != NULL)
                testVisualize.drawRect(chunkGrid[a][b]->x_min, chunkGrid[a][b]->y_min, map->chunkSize, map->chunkSize, sf::Color::Black);
}

bool vae::vsm::chunk::Node::setX(coordType to){
    Chunklet::Ptr targetChunk = getMap()->getChunklet( getMap()->translateToChunk(to), getChunklet()->chunk_y);
    if( targetChunk.get() != getChunklet().get() ){
        if(targetChunk->insert(this)) {
            std::cout << "Insert failure." << std::endl;
            return true;
        }
        else
            this->chunk = targetChunk;
    }
    x.setPos(to);
    return false;
}
bool vae::vsm::chunk::Node::setY(coordType to){
    Chunklet::Ptr targetChunk = getMap()->getChunklet(getChunklet()->chunk_x, getMap()->translateToChunk(to));
    if( targetChunk.get() != getChunklet().get() ){
        if(targetChunk->insert(this)) {
            std::cout << "Insert failure." << std::endl;
            return true;
        }
        else
            this->chunk = targetChunk;
    }
    y.setPos(to);
    return false;
}

bool vae::vsm::chunk::NodeList::setPos(coordType to)
{
/*    Chunklet::Ptr targetChunk;
    if (to > node.getChunklet()->x_max) {
        //TODO: Ensure we're moving within bounds of the map
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
