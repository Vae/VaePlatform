#include <boost/asio/io_context.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include "BoostSerializationTest.h"
#include "PostgreSQLTest.h"

#include "vae/vsm/TestVisualize.h"
#include "vae/vsm/BasicMap.h"

#include "LuaState.h"

#include "vae/defs.hpp"

#include "vae/vsm/Map.h"
#include "vae/react/Action.h"
#include "olib/concurrentqueue.h"

#include "vae/log.h"
#include "vae/LogService.h"
#include "vae/Datastore.h"
#include "vae/TestService.h"
#include "vae/LogStore.h"
#include "vae/vsm/TestVisualize.h"
/**
 * Basic mapping
 *  Node OK
 *  Viewport
 *  Map
 *   Maps are split into chunks: scripts should not be chunk-aware!
 *  Composer (service)
 * Actions: sub/pub
 *  Actor (Action)
 *   Actions we can perform:
 *  *
 *  Observer
 * Thread testing
 * Scripting
 *  Basic scripting interface
 * Interactions
 *  Simple stats
 *  Simple battle testing
 *
 *  At any point:
 *   Logging (service) OK
 *   Database (service)
 */

//#define LOG(level) vl::level(logInstance).start(__FILE__, __LINE__)
#include "olib/influxdb.hpp"

vae::ServerInstance vae::serverInstance;

template <class Duration>
using sys_time = std::chrono::time_point<std::chrono::system_clock, Duration>;
using sys_nanoseconds = sys_time<std::chrono::nanoseconds>;
#include<iostream>
using namespace std;

void startDB(){
}

int main() {
    //Fall-off-the-bone Steak: (2/3)21 3 or 2 hours exposed to smoke, 2 hours in foil, 1 hour. 250f

    boost::asio::io_service ioService;

    //window will be deleted by TestService
    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(800, 600), "Vaewyn Server Mapping Visualizer");

    influxdb_cpp::server_info si("192.168.99.10", 8086, "vae", "vae_server", "pass");
    //LogStore logStore();
    LogService logService;

    auto f = [&](const char* system, long time, const char* level, const float lv, const char* file, const int line, const char* function, const std::string &message){
        logService.log(system, time, level, lv, file, line, function, message);
    };
    logInstance.setLoggingFunction(f);
    logService.setWriterFunction([&](const char* system, long time, const char* level, const float lv, const char* file, const int line, const char* function, const std::string &message) {
        //auto l = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        //cout << l << "   \n" << time << endl;
        //cout DEB(system) DEB(lv) DEB(level) DEB(file) DEB(line) DEB(function) DEB(message) << endl;
        int r = 0;
        if(lv > 0)
            r = influxdb_cpp::builder()
                    .meas("vaewyn")
                    //.tag("file", file)
                    //.tag("system", system)
                    .field("system", std::string(system))
                    .field("level", lv)
                    .field("level name", std::string(level))
                    .field("file", std::string(file))
                    .field("line", line)
                    .field("function", std::string(function))
                    .field("message", message)
                    .timestamp(time)
                    .post_http(si);
        if(r != 0)
            cout << "InfluxDB write error: " << r << endl;
    });
    LOG(Debug) << "Start.";

    //LuaState luaState;
    ////    void log(int level, const char* file, int line, const char* function, std::string message){
    //luaL_dostring(
    //        luaState.getLua().lua_state(),
    //        "print(\"Hello bub\")"
    //);
    //log(int level, const char* file, int line, const char* function, std::string message)
    //state.do_string("cppObj = CppObject:new() cppObj:TestFunction1()");
    //luaState.getLua().do_string("t = LuaState:new() t:test()"
    //                            "t:log(1, \"abc\", 2, \"xyz\", \"qwe\")");

    //Datastore ds;
    //ds.connect("dbname = test1 user = root password = pass hostaddr = 192.168.99.10 port = 25432");

    TestService *testService = new TestService(ioService, window);
    //vae::vsm::TestVisualize testVisualize("test");

    //std::cout << "BoostSerializationTest: " << BoostSerializationTest::boostSerializationTest() << std::endl;
    //std::cout << "PostgreSQLTest: " << PostgreSQLTest::postgreSQLTest() << std::endl;

    std::vector<std::thread> threads;
    auto count = std::thread::hardware_concurrency() * 2;
    //count = 1;
    cout << "Thread count: " << count << endl;
    LOG(Info) << "Thread count: " << count;

    try{
        //How to do this without making run() last forever?
        //boost::asio::signal_set signals(ioService, SIGINT, SIGTERM);
        //signals.async_wait([&](auto, auto){ cout << "Kill/term" << endl; vae::serverInstance.stop(); });

        for(int n = 0; n < count; ++n){
            threads.emplace_back([&]
                                 {
                                     ioService.run();
                                 });
        }
        for(auto& thread : threads){
            if(thread.joinable()){
                thread.join();
            }
        }
    }
    catch (std::exception& e){
        std::printf("Exception: %s\n", e.what());
    }
    LOG(Debug) << "delete TestService.";
    delete testService;
    //LOG(Debug) << "delete sf::RenderWindow.";
    //delete window;
    cout << "Waiting on global logging service to finish..." << endl;
    //Nothing should be doing any logging after this is called!
    logService.shutdown();
    cout << "Stop" << endl;

    /*
    moodycamel::ConcurrentQueue<int> q;
    q.enqueue(25);
    int item;
    bool found = q.try_dequeue(item);
    assert(found && item == 25);
     */

    return 0;
}
