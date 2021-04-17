//
// Created by protocol on 4/14/21.
//

#include "ChunkMap.h"


bool vae::vsm::chunk::Chunklet::insert(Node *node){
    //TODO: Ensure we're inserting into a valid location and valid for the node type.
    //To do this, we'll need to have a bunch of high-level logic in place.

    if(node->getChunklet() != NULL)
        node->getChunklet().get()->remove(node);

    nodes.push_back(node);

    //TODO: Inform all viewports of the new node (if applicable)

    return false;
}


bool vae::vsm::chunk::Node::moveX(coordType to)
{
    //Are we walking off a chunk? (moving right)
    if(to > chunk.get()->x_max){
        //TODO: Ensure we're moving  within bounds of the map

        Chunklet::Ptr nextChunk = map.get()->map[chunk.get()->chunk_x + 1][chunk.get()->chunk_y];
        nextChunk->insert(this);
    }
    if(to < chunk.get()->x_min){
        //TODO: Ensure we're moving within bounds of the map

        Chunklet::Ptr nextChunk = map.get()->map[chunk.get()->chunk_x - 1][chunk.get()->chunk_y];
        nextChunk->insert(this);
    }
    if(to > chunk.get()->y_max){
        //TODO: Ensure we're moving  within bounds of the map

        Chunklet::Ptr nextChunk = map.get()->map[chunk.get()->chunk_x][chunk.get()->chunk_y + 1];
        nextChunk->insert(this);
    }
    if(to < chunk.get()->y_min){
        //TODO: Ensure we're moving  within bounds of the map

        Chunklet::Ptr nextChunk = map.get()->map[chunk.get()->chunk_x][chunk.get()->chunk_y - 1];
        nextChunk->insert(this);
    }
}
