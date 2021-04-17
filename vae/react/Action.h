//
// Created by protocol on 4/16/21.
//

#ifndef BOOSTTESTING_ACTION_H
#define BOOSTTESTING_ACTION_H

#include <memory>

namespace vae {
    namespace react {
/**
 * Contains all data needed by a viewport to witness an action.
 * "all data" because each time we need to ask a node something, we stall the node.
 */
class Observation{
};

/**
 * Contains all data needed by a Node to do something.
 * "all data" because each node is reacting on its own thread.
 *
 * For now, everything is public since this should be a read-only class after construction.
 */
class Action{
public:
    typedef std::shared_ptr<Action> Ptr;
};

    }   //namespace react
}   //namespace vae
#endif //BOOSTTESTING_ACTION_H
