//
// Created by protocol on 5/22/21.
//

#ifndef BOOSTTESTING_NODEACTIONS_H
#define BOOSTTESTING_NODEACTIONS_H

#include <boost/exception/detail/shared_ptr.hpp>
#include "Map.h"

namespace vae::vsm::chunk {

    class NodeActionMove: public vae::vsm::chunk::NodeAction{
    public:
        const vae::vsm::chunk::Vector newPos;
        NodeActionMove(vae::vsm::chunk::Node::Ptr owner, Vector &newPos): NodeAction(owner), newPos(newPos){}
    };

    class NodeActionAppearance: public NodeAction{
    public:
        NodeAppearance::Ptr nodeAppearance;
        NodeActionAppearance(vae::vsm::chunk::Node::Ptr owner, NodeAppearance::Ptr nodeAppearance): NodeAction(owner), nodeAppearance(nodeAppearance){}
    };

    class NodeActionAppear: public NodeAction{
    public:
        NodeAppearance::Ptr nodeAppearance;
        NodeActionAppear(vae::vsm::chunk::Node::Ptr owner, NodeAppearance::Ptr nodeAppearance): NodeAction(owner), nodeAppearance(nodeAppearance){}
    };

    class NodeActionExit: public NodeAction{
    public:
        NodeActionExit(vae::vsm::chunk::Node::Ptr owner): NodeAction(owner){}
    };


}

#endif //BOOSTTESTING_NODEACTIONS_H
