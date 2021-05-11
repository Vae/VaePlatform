cmake_minimum_required(VERSION 3.17)
project(BoostTesting)

set(CMAKE_CXX_STANDARD 17)

set(Boost_INCLUDE_DIR /usr/local/lib)
set(Boost_LIBRARY_DIR /usr/local/include)
find_package(Boost COMPONENTS serialization thread REQUIRED)

link_directories(${Boost_LIBRARY_DIR})

if(Boost_FOUND)
    message(${PROJECT_SOURCE_DIR})
    include_directories(${Boost_INCLUDE_DIR} /usr/include)
    include_directories(${PROJECT_SOURCE_DIR})


    #lua
    find_package(Lua REQUIRED)
    include_directories(/usr/include/lua5.2)
    link_libraries(lua5.2)
    #sol2: Requires some version of LUA 5.1+
    include_directories(${PROJECT_SOURCE_DIR}/olib/sol2/include)
    add_subdirectory("${PROJECT_SOURCE_DIR}/olib/sol2" "${PROJECT_SOURCE_DIR}/olib/sol2/sol2_output")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpqxx -lpq")
    set(PQXX /usr/include/pqxx)
    find_library(PQXX_LIB pqxx)
    find_library(PQ_LIB pq)

    link_libraries(sfml-graphics sfml-window sfml-system)
    #add_library(/usr/lib/x86_64-linux-gnu/cmake/SFML)

    file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

    include_directories(${PROJECT_SOURCE_DIR})

    add_executable(VaeTesting
            main.cpp
            vae/defs.hpp
            vae/defs.cpp
            BoostSerializationTest.cpp
            BoostSerializationTest.h
            PostgreSQLTest.cpp
            PostgreSQLTest.h
            LuaState.cpp
            LuaState.h
            vae/vsm/SkipMap.cpp
            vae/vsm/SkipMap.h
            vae/vsm/TestVisualize.cpp
            vae/vsm/TestVisualize.h
            vae/vsm/ChunkMap.cpp
            vae/vsm/ChunkMap.h
            vae/vsm/BasicMap.cpp
            vae/vsm/BasicMap.h
            vae/react/Action.cpp
            vae/react/Action.h
            vae/react/Observer.cpp
            vae/react/Observer.h
            vae/react/EntityActions.cpp
            vae/react/EntityActions.h
            vae/vsm/Map.cpp
            vae/vsm/Map.h
            vae/ThreadManager.cpp
            vae/ThreadManager.h
            log/logger_service.cpp
            log/logger_service.hpp
            vae/log.cpp
            vae/log.h
            vae/Datastore.cpp
            vae/Datastore.h
            vae/LogService.cpp
            vae/LogService.h
            vae/TestService.cpp
            vae/TestService.h
            olib/influxdb.hpp
            vae/LogStore.cpp
            vae/LogStore.h
            vae/StressService.cpp
            vae/StressService.h
            vae/test.cpp
            vae/test.h
            vae/Engine.cpp
            vae/Engine.h
            vae/defs.cpp
            vae/entity/Entity.cpp
            vae/entity/Entity.h
            vae/Service.h
            vae/HereGoes.cpp
            vae/HereGoes.h
            vae/Service.cpp
            vae/GuiService.cpp
            vae/GuiService.h
            vae/entity/EntityTestService.cpp
            vae/entity/EntityTestService.h vae/react/Ability.cpp vae/react/Ability.h vae/react/EntityAbilities.cpp vae/react/EntityAbilities.h)

    target_link_libraries(VaeTesting ${Boost_LIBRARIES} ${PQXX_LIB} ${PQ_LIB})
# required for threads
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lboost_thread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)

endif()
