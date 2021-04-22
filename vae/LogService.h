//
// Created by protocol on 4/21/21.
//

#ifndef BOOSTTESTING_LOGSERVICE_H
#define BOOSTTESTING_LOGSERVICE_H


#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/scoped_ptr.hpp>
#include <iostream>


template<typename impl_type>
class LogService {
    boost::asio::io_service &ios;
public:
    //typedef vl::LogEngine impl_type;

    LogService(boost::asio::io_service &ios):
            ios(ios),
            work_io_service_(),
            work_(new boost::asio::io_service::work(work_io_service_)),
    work_thread_(new boost::thread(boost::bind(&boost::asio::io_service::run, &work_io_service_))){
    }

    void use_file(impl_type& /*impl*/, const std::string& file){
        // Pass the work of opening the file to the background thread.
        work_io_service_.post(boost::bind(&LogService::use_file_impl, this, file));
    }

    /// Log a message.
    void log(const std::string& message){
        // Pass the work of opening the file to the background thread.
        work_io_service_.post(boost::bind(&LogService::log_impl, this, message));
    }
private:
    /// Private io_service used for performing logging operations.
    boost::asio::io_service work_io_service_;

    /// Work for the private io_service to perform. If we do not give the
    /// io_service some work to do then the io_service::run() function will exit
    /// immediately.
    boost::scoped_ptr<boost::asio::io_service::work> work_;

    /// Thread used for running the work io_service's run loop.
    boost::scoped_ptr<boost::thread> work_thread_;

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
    void log_impl(const std::string& text)
    {
        //  if(ofstream_.is_open())
        //	  ofstream_ << text << std::endl;
        //  else
        std::cout << text << std::endl;
    }
};

#endif //BOOSTTESTING_LOGSERVICE_H
