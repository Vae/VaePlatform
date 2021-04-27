#include "defs.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include "BoostSerializationTest.h"
#include "PostgreSQLTest.h"

#include "vae/vsm/TestVisualize.h"
#include "vae/vsm/BasicMap.h"

#include "LuaState.h"

#include "vae/vsm/Map.h"
#include "vae/react/Action.h"
#include "olib/concurrentqueue.h"

#include "vae/log.h"
#include "vae/LogService.h"
#include "vae/Datastore.h"
#include "vae/TestService.h"
#include "vae/LogStore.h"

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
template <class Duration>
using sys_time = std::chrono::time_point<std::chrono::system_clock, Duration>;
using sys_nanoseconds = sys_time<std::chrono::nanoseconds>;
#include<iostream>
using namespace std;
int main() {
    boost::asio::io_service ioService;
    influxdb_cpp::server_info si("192.168.99.10", 8086, "vae", "vae_server", "pass");
    //LogStore logStore();
    LogService logService(ioService);

    auto f = [&](const char* system, long time, const char* level, const float lv, const char* file, const int line, const char* function, const std::string &message){
        logService.log(system, time, level, lv, file, line, function, message);
    };

    logInstance.setLoggingFunction(f);

    logService.setWriterFunction([&](const char* system, long time, const char* level, const float lv, const char* file, const int line, const char* function, const std::string &message) {
        //auto l = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        //cout << l << "   \n" << time << endl;
        //cout DEB(system) DEB(lv) DEB(level) DEB(file) DEB(line) DEB(function) DEB(message) << endl;
        int r = influxdb_cpp::builder()
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

    LuaState luaState;
    //    void log(int level, const char* file, int line, const char* function, std::string message){
    luaL_dostring(
            luaState.getLua().lua_state(),
            "print(\"Hello bub\")"
    );
    //log(int level, const char* file, int line, const char* function, std::string message)
    //state.do_string("cppObj = CppObject:new() cppObj:TestFunction1()");
    luaState.getLua().do_string("t = LuaState:new() t:test()"
                                "t:log(1, \"abc\", 2, \"xyz\", \"qwe\")");
    Datastore ds;
    ds.connect("dbname = test1 user = root password = pass hostaddr = 192.168.99.10 port = 25432");

    TestService testService(ioService);

    //std::cout << "BoostSerializationTest: " << BoostSerializationTest::boostSerializationTest() << std::endl;
    //std::cout << "PostgreSQLTest: " << PostgreSQLTest::postgreSQLTest() << std::endl;

    try
    {
        boost::asio::signal_set signals(ioService, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto){ ioService.stop(); });

        ioService.run();
    }
    catch (std::exception& e)
    {
        std::printf("Exception: %s\n", e.what());
    }


    /*
    moodycamel::ConcurrentQueue<int> q;
    q.enqueue(25);
    int item;
    bool found = q.try_dequeue(item);
    assert(found && item == 25);
     */

    return 0;
}
