#include "defs.h"

#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include "BoostSerializationTest.h"
#include "PostgreSQLTest.h"

#include "vae/vsm/TestVisualize.h"
#include "vae/vsm/BasicMap.h"

#include "vae/vsm/Map.h"
#include "vae/react/Action.h"
#include "olib/concurrentqueue.h"

#include "vae/log.h"
#include "vae/LogService.h"
#include "vae/Datastore.h"
#include "vae/TestService.h"

/**
 * Basic mapping
 *  Node
 *  Viewport
 *  Map
 *  Composer (service)
 * Actions: sub/pub
 *  Actor
 *  Observer
 * Thread testing
 * Scripting
 *  Basic scripting interface
 * Interactions
 *  Simple stats
 *  Simple battle testing
 *
 *  At any point:
 *   Logging (service)
 *   Database (service)
 */

//#define LOG(level) vl::level(logInstance).start(__FILE__, __LINE__)

int main() {
    boost::asio::io_service ioService;

    LogService<vl::LogEngine> logService(ioService);

    auto f = [&](const std::string &message){
        std::cout << message << std::endl;
    };
    logInstance.setLoggingFunction(f);

    LOG(Debug) << "Start.";

    Datastore ds;
    ds.connect("dbname = test1 user = root password = pass hostaddr = 192.168.99.10 port = 25432");

    TestService testService(ioService);

    //services::FILELog::ReportingLevel() = logDEBUG3;
    //std::cout << "BoostSerializationTest: " << BoostSerializationTest::boostSerializationTest() << std::endl;
    //std::cout << "PostgreSQLTest: " << PostgreSQLTest::postgreSQLTest() << std::endl;

    try
    {
        //boost::asio::io_context ioContext(1);

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
