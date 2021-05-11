//
// Created by protocol on 5/11/21.
//

#include "EntityAbilities.h"

void vae::react::AbilityHear::hear(std::string message){
    LOG(Debug) << "Hear: " << message;
}
void vae::react::AbilityHear::enlist(LuaState &s){

    //https://sol2.readthedocs.io/en/latest/tutorial/cxx-in-lua.html

    sol::usertype<vae::react::AbilityHear> player_type = s.getLua().new_usertype<vae::react::AbilityHear>(
            "AbilityHear", sol::constructors<vae::react::AbilityHear()>(),
            "hear", &vae::react::AbilityHear::hear
    );
    s.getLua()["abilityHear"] = this;
}

/*
template<class P, class C> //Producer, Consumer
void vae::react::AbilityHear<P, C>::hearX(std::string message){
}

template<class P, class C> //Producer, Consumer
void vae::react::AbilityHear<P, C>::enlist(LuaState &s){
//    sol::usertype<vae::react::AbilityHear<P, C>> player_type = s.getLua().new_usertype<vae::react::AbilityHear<P, C>>(
//            "AbilityHear", sol::constructors<vae::react::AbilityHear<P, C>()>(),
//            "hear", &vae::react::AbilityHear<P, C>::hear
//    );
}

*/