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
        template<class P, class C> //Producer, Consumer
        class Action: private boost::noncopyable{
        public:
            typedef std::shared_ptr<P> PPtr;
            typedef std::shared_ptr<C> CPtr;
        public:
            const PPtr source;
            typedef std::shared_ptr<Action> Ptr;
            Action(){}
            Action(PPtr source): source(source){}
            //Action(PPtr source, std::function<void(PPtr)> f): source(source){}
            virtual void witness(CPtr c)=0;
        };

        template<class P, class C> //Producer, Consumer
        class Ability: private boost::noncopyable{
            P &p;
            Ability(P &owner): p(owner){
            }
            P &getOwner() { return p; }
        };

        template<class P, class C> //Producer, Consumer
        class AbilityHear: public Ability<P, C>{
        public:
            AbilityHear(P &owner): Ability<P, C>(owner){}

            void hear(std::string message);
        };

        template<class P, class C> //Producer, Consumer
        class ActionSpeak: public Action<P, C>{
            std::string message;
        public:
            ActionSpeak(std::string message):message(message){}
            virtual void witness(std::shared_ptr<C> c){
                AbilityHear<P, C> *hearing = c->getAbility("hear");
                if(hearing){
                    hearing->hear(message);
                }
            }
        };

        /**
         * What kind of actions could exist and what is need for that action to take place?
         *   What would be seen by observers(viewport)?
         *
         * Move action: RO? Node attempts to move some distance. Collision detection.
         *   Observers see a move action if collision did not occur.
         * Talk action: RO.
         * Interact: RW. Attack, spells. Needs exclusive access to chunklet and all objects.
         * Map update: RO? Tiles, overlay
         * BBS: RO. Posting, reading
         * Stat/char modification: RO.
         *
         *
         */

    }   //namespace react
}   //namespace vae
#endif //BOOSTTESTING_ACTION_H
