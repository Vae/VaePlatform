//
// Created by protocol on 4/16/21.
//

#ifndef BOOSTTESTING_ACTION_H
#define BOOSTTESTING_ACTION_H

#include <memory>
#include <boost/noncopyable.hpp>
#include <functional>

namespace vae {
    namespace react {
        /**
         * Contains all data needed by a viewport to witness an action.
         * "all data" because each time we need to ask a node something, we stall the node.
         */
        template<class P, class C>  //Producer, Consumer
        class Observation{
            typedef std::shared_ptr<P> PPtr;
            typedef std::shared_ptr<C> CPtr;
        public:
            const PPtr source;
            Observation(PPtr source, std::function<void(PPtr)> f): source(source){}
        };

        /**
         * Contains all data needed by Action to do something to an Actor/Node.
         * "all data" because each node is reacting on its own thread.
         *
         * For now, everything is public since this should be a read-only class after construction.
         */
        template<class T>
        class Action: private boost::noncopyable{
            typedef std::shared_ptr<T> TPtr;
        public:
            const TPtr source;
            typedef std::shared_ptr<Action> Ptr;
            Action(TPtr source, std::function<void(TPtr)> f): source(source){}
        };

    }   //namespace react
}   //namespace vae
#endif //BOOSTTESTING_ACTION_H
