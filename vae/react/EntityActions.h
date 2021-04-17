//
// Created by protocol on 4/16/21.
//

#ifndef BOOSTTESTING_ENTITYACTIONS_H
#define BOOSTTESTING_ENTITYACTIONS_H

#include "Action.h"

namespace vae {
    namespace react {

class EntityTalk: public Action {
public:
    const std::string message;
    const std::string source;
    bool enqueued = false;
    EntityTalk(std::string who_is_speaking, std::string what_they_said): source(who_is_speaking), message(who_is_speaking){}

    void enqueue(){}
};

    }   //namespace react
}   //namespace vae
#endif //BOOSTTESTING_ENTITYACTIONS_H
