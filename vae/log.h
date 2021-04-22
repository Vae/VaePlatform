//
// Created by protocol on 4/20/21.
//

#ifndef BOOSTTESTING_LOG_H
#define BOOSTTESTING_LOG_H
#include <sstream>
#include <ctime>
#include <iomanip>
#include <boost/noncopyable.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <iostream>
#include <sstream>
#include "../olib/concurrentqueue.h"
#include "LogService.h"

//      Log a single line:
//    LOG(Debug) << "Start.";

//      Log multiple:
//    auto d = LOG_START(Debug);
//    LOGA(d) << "Info and ";
//    LOGA(d) << "more info";
//    d.finalize();


#define LOG(level) vl::level(logInstance).start(__FILE__, __LINE__)
#define LOG_START(level) vl::level(&vl::logInstance)
#define LOGA(instance) instance.append()

namespace vl {

    typedef float level_t;
//new entries: NETWORK_SECURITY_LOW, NETWORK_SECURITY_HIGH
    enum Level {logEXCEPTION, logERROR, logSECURITY, logWARNING, logINFO, logDEBUG, logDEBUG1, logDEBUG2, logDEBUG3, logDEBUG4, logLOWEST};
    inline std::string NowTimeX()
    {
//        auto t = std::time(nullptr);
//        auto tm = *std::localtime(&t);
//        auto time = std::put_time(&tm, "%F %H:%M:%S");
//        std::ostringstream wss();
//        wss << time;
//        return wss.str();
//        std::string str = std::put_time(&tm, "%F %H:%M:%S");
//        return str;
        return "";
    }
    std::string FormatTime(boost::posix_time::ptime now);
    inline std::string NowTime();
    class LogEngine;

    class LogLevel: private boost::noncopyable{
    private:
        bool sent;
    protected:
        bool codeDetails;
    public:
        const char* name;
        const float level;
        std::ostringstream os;
        LogEngine &logEngine;

        LogLevel(LogEngine &logEngine, level_t level, const char *name): level(level), name(name), logEngine(logEngine), sent(false){}
        //std::string getName() const { return name; }
        level_t getLevel() const { return level; }
        std::ostringstream &start(const char *filename, const int line);
        std::ostringstream &start();
        std::ostringstream &append(){
            assert(sent == false);
            return os;
        }
        void finalize();
        ~LogLevel();
    };

    class Debug: public LogLevel{
    public:
        Debug(LogEngine &logEngine): LogLevel(logEngine, 0, "debug") {
            codeDetails = true;
        }
    };

    class Info: public LogLevel{
    public:
        Info(LogEngine &logEngine): LogLevel(logEngine, 10, "info") {
            codeDetails = false;
        }
    };

    class Warn: public LogLevel{
    public:
        Warn(LogEngine &logEngine): LogLevel(logEngine, 100, "warning") {
            codeDetails = true;
        }
    };
    typedef Warn Warning;

    class Error: public LogLevel{
    public:
        Error(LogEngine &logEngine): LogLevel(logEngine, 1000, "warning") {
            codeDetails = true;
        }
    };
    typedef Error Err;

    class Security: public LogLevel{
    public:
        Security(LogEngine &logEngine): LogLevel(logEngine, 10000, "security") {
            codeDetails = true;
        }
    };

    class LogEngine : private boost::noncopyable{
    public:
        const char *systemName;
        LogEngine(const char * systemName);
        LogEngine(const char * systemName, std::function<void(const std::string&)> f);
        virtual ~LogEngine();
        std::ostringstream &get(LogLevel);
        void reportingLevelTo(LogLevel &l){
            reportingLevel = l.getLevel();
            ignoreReportinLevel = false;
        }
        void reportingLevelReset(){
            ignoreReportinLevel = true;
        }
        void setLoggingFunction(std::function<void(const std::string&)> f){
            useInserter = true;
            inserter = f;
        }
        //std::ostringstream &get(Level);
        //Log &Set(const char *file, int line);
        void insert(const std::string &dis);
    public:
        Level &ReportingLevel();
    protected:
        std::ostringstream os;
        const char *file;
        int line;
    private:
        std::function<void(const std::string&)> inserter;
        bool useInserter = false;
        void defaultInsert(const std::string &dis){
            std::cout << dis << std::endl;
        }
        moodycamel::ConcurrentQueue<std::string> log_queue;
        bool ignoreReportinLevel = true;
        level_t reportingLevel = 0;
        LogEngine(const LogEngine &);
        LogEngine &operator=(const LogEngine &);
    };
}

extern vl::LogEngine logInstance;

#endif //BOOSTTESTING_LOG_H
