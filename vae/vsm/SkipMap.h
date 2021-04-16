//
// Created by protocol on 4/14/21.
//

#ifndef BOOSTTESTING_SKIPMAP_H
#define BOOSTTESTING_SKIPMAP_H


namespace vae {
    namespace vsm {
class SkipIndex;
typedef int coordType;

class SkipNode {
public:
    coordType pos;
    void *payload;
    SkipNode *nextNode, *prevNode;
    SkipIndex *nextIndex;
};

class SkipIndex {

};

class SkipArray{
    int levels;
    int maxNodesPerLevel;

    void insert(SkipNode *node){

    }
};

class SkipMap {
public:
    SkipMap(){
    }
};

    } //namespace vsm
} //namespace vae
#endif //BOOSTTESTING_SKIPMAP_H
