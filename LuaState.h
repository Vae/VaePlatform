//
// Created by protocol on 4/14/21.
//

#ifndef BOOSTTESTING_LUASTATE_H
#define BOOSTTESTING_LUASTATE_H

#include <lua.hpp>
#include <sol/sol.hpp>  //this must come after include<lua.hpp>
#include "vae/log.h"

class LuaState {
    sol::state lua;
    void logDebug(const char* file, int line, const char* function, std::string message){
        vl::Debug(logInstance).start(file, line, function) << message;
        //std::cout << message << std::endl;
    }
    void log(const char* file, int line, const char* function, std::string message){
        vl::Debug(logInstance).start(file, line, function) << message;
        //std::cout << message << std::endl;
    }
    void logError(const char* file, int line, const char* function, std::string message){
        vl::Debug(logInstance).start(file, line, function) << message;
        //std::cout << message << std::endl;
    }

public:
    void test(){
        std::cout << "heyyyyyyyyyyyy" << std::endl;
    }
    sol::state &getLua() { return lua; }
    LuaState(){
        lua.open_libraries(sol::lib::base);
        sol::usertype<LuaState> player_type = lua.new_usertype<LuaState>(
                "LuaState", sol::constructors<LuaState()>(),
                "test", &LuaState::test,
                "log", &LuaState::log,
                "logDebug", &LuaState::logDebug,
                "logError", &LuaState::logError
                );
    }
};

#endif //BOOSTTESTING_LUASTATE_H
