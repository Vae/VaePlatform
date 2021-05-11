//
// Created by protocol on 5/11/21.
//

#ifndef BOOSTTESTING_ABILITY_H
#define BOOSTTESTING_ABILITY_H

#include <boost/noncopyable.hpp>
#include "../../LuaState.h"

namespace vae {
    namespace react {

        typedef std::string AbilityNameType;

        //template<class P, class C> //Producer, Consumer
        class Ability: private boost::noncopyable{
        public:
            //typedef std::shared_ptr<Ability<P, C>> Ptr;
            typedef std::shared_ptr<Ability> Ptr;
            //Ability(P &owner): p(owner){}
            //P &getOwner() { return p; }
            virtual void enlist(LuaState &s)=0;
            virtual const AbilityNameType getName() const=0;
        };

        template<class P, class C> //Producer, Consumer
        class AbilityFactory{
            //Ability *getInstance(std::string dis)
        };

    }   //namespace react
}   //namespace vae
#endif //BOOSTTESTING_ABILITY_H
