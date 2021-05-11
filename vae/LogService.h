//
// Created by protocol on 4/21/21.
//

#ifndef BOOSTTESTING_LOGSERVICE_H
#define BOOSTTESTING_LOGSERVICE_H


#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/detail/thread_group.hpp>

#include "defs.hpp"

namespace lv{
    static const boost::posix_time::ptime time_epoch(boost::gregorian::date(1970, 1, 1));
}
struct LogTemplate{
    const char* system;
    const float lv;
    const char* file;
    const int line;
    bool operator < (const LogTemplate &a) const { return !(this->system == a.system && this->lv == a.lv && this->file == a.file && this->line == a.line); }
//        LogTemplate(const char* system, long time, const float lv, const char* file, const int line, const char* function):
//            system(system), time(time), lv(lv), file(file), line(line), function(function){}
};
class LogService {
    //boost::asio::io_service &ios;
    //std::map<LogTemplate, int> seen;
    //level, system, file, line -> count
    vae::SharedResource<std::map<std::string, std::map<std::string, std::map<std::string, std::map<const int, int>>>>, boost::mutex> logEntryHx;
    //std::map<std::string, std::map<std::string, std::map<std::string, std::map<const int, int>>>> seenx;
public:
    //typedef vl::LogEngine impl_type;
    typedef std::function<void(const char* system, long time, const char* level, const float lv, const char* file, const int line, const char* function, const std::string &message)> sinkFunctionType;

    LogService():
            //ios(ios),
            work_io_service_(),
            work_(new boost::asio::io_service::work(work_io_service_)),
            //work_thread_(new boost::thread(new boost::thread(boost::bind(&boost::asio::io_service::run, &work_io_service_)))),
            useSinks(false),
            coutStrand(work_io_service_)
            {
        for(int a = 0; a < 1; ++a)
            threadGroup.add_thread(new boost::thread(boost::bind(&boost::asio::io_service::run, &work_io_service_), std::string("LogService_"+a)));
    }
    ~LogService(){
        work_.reset();
        //work_thread_->join_all();
        threadGroup.join_all();
    }

    //Nothing should be doing any logging after this is called!
    void shutdown(){
    }

    void setWriterFunction(sinkFunctionType to){
        sink = to;
        useSinks = true;
    }

    /// Log a message.
    /*
    void log(const std::string& message){
        // Pass the work of opening the file to the background thread.
        work_io_service_.post(boost::bind(&LogService::log_impl, this, message));
    }
    */
    void log(const char* system, long time, const char* level, const float lv, const char* file, const int line, const char* function, const std::string message){
        work_io_service_.post(boost::bind(&LogService::log_impl, this, system, time, level, lv, file, line, function, message));
        //ios.post(boost::bind(&LogService::log_impl, this, system, time, level, lv, file, line, function, message));
    }
private:
    /// Private io_service used for performing logging operations.
    boost::asio::io_service work_io_service_;

    /// Work for the private io_service to perform. If we do not give the
    /// io_service some work to do then the io_service::run() function will exit
    /// immediately.
    boost::scoped_ptr<boost::asio::io_service::work> work_;

    /// Thread used for running the work io_service's run loop.
    //boost::scoped_ptr<boost::thread_group> work_thread_;
    boost::thread_group threadGroup;

    ///WORKER THREAD
    /// Where logs go to be written: this will be accessed by the worker thread!
    sinkFunctionType sink;
    bool useSinks;

    /// Helper function used to open the output file from within the private
    /// io_service's thread.
    void use_file_impl(const std::string& file)
    {
        //ofstream_.close();
        //ofstream_.clear();
        //ofstream_.open(file.c_str());
    }

    /// Helper function used to log a message from within the private io_service's
    /// thread.
    //void log_impl(const std::string& text);
    void log_impl(const char* system, long time, const char* level, const float lv, const char* file, const int line, const char* function, const std::string &message);

    boost::asio::io_context::strand coutStrand;
    void cout_impl(const char* system, long time, const char* level, const float lv, const char* file, const int line, const char* function, const std::string message);
};

#endif //BOOSTTESTING_LOGSERVICE_H
