//
// Created by protocol on 5/11/21.
//

#ifndef BOOSTTESTING_ENTITYABILITIES_H
#define BOOSTTESTING_ENTITYABILITIES_H

#include "Ability.h"

namespace vae {
    namespace react {
        //template<class P, class C> //Producer, Consumer
        class AbilityHear: public Ability{
        public:
            void enlist(LuaState &s);
            const vae::react::AbilityNameType  getName() const { return "hear"; }
            //AbilityHear(P &owner): Ability<P, C>(owner){}
            //AbilityHear(){}
            void hear(std::string message);
        };
    }   //namespace react
}   //namespace vae

#endif //BOOSTTESTING_ENTITYABILITIES_H
