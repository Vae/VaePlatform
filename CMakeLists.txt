cmake_minimum_required(VERSION 3.17)
project(BoostTesting)

set(CMAKE_CXX_STANDARD 17)

set(Boost_INCLUDE_DIR /usr/local/lib)
set(Boost_LIBRARY_DIR /usr/local/include)
find_package(Boost COMPONENTS serialization thread REQUIRED)

link_directories(${Boost_LIBRARY_DIR})

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR} /usr/include/lua5.3 /usr/include/luabind /usr/include)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpqxx -lpq")
    set(PQXX /usr/include/pqxx)
    find_library(PQXX_LIB pqxx)
    find_library(PQ_LIB pq)

    link_libraries(sfml-graphics sfml-window sfml-system)
    #add_library(/usr/lib/x86_64-linux-gnu/cmake/SFML)

    file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

    add_executable(VaeTesting main.cpp BoostSerializationTest.cpp BoostSerializationTest.h PostgreSQLTest.cpp PostgreSQLTest.h LuaBindTest.cpp LuaBindTest.h vae/vsm/SkipMap.cpp vae/vsm/SkipMap.h vae/vsm/TestVisualize.cpp vae/vsm/TestVisualize.h vae/vsm/ChunkMap.cpp vae/vsm/ChunkMap.h vae/vsm/BasicMap.cpp vae/vsm/BasicMap.h vae/react/Action.cpp vae/react/Action.h vae/react/Observer.cpp vae/react/Observer.h vae/react/EntityActions.cpp vae/react/EntityActions.h vae/vsm/Map.cpp vae/vsm/Map.h vae/ThreadManager.cpp vae/ThreadManager.h log/logger_service.cpp log/logger_service.hpp vae/log.cpp vae/log.h vae/Datastore.cpp vae/Datastore.h vae/LogService.cpp vae/LogService.h vae/TestService.cpp vae/TestService.h)

    target_link_libraries(VaeTesting ${Boost_LIBRARIES} ${PQXX_LIB} ${PQ_LIB})
# required for threads
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lboost_thread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)

endif()