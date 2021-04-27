//
// Created by protocol on 4/21/21.
//

#include "LogService.h"
#include <iostream>
#include <boost/format.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
/*
template<typename impl_type, typename storage_type>
void LogService<impl_type, storage_type>::log_impl(const std::string& text){
    //  if(ofstream_.is_open())
    //	  ofstream_ << text << std::endl;
    //  else
    std::cout << text << std::endl;
}*/

void LogService::log_impl(const char* system, long time, const char* level, const float lv, const char* file, const int line, const char* function, const std::string &message){

    boost::format f2("%02u");

    boost::posix_time::ptime eventTime = boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1)) + boost::posix_time::milliseconds(time / 1000000);

    std::cout

        //<< boost::posix_time::to_iso_string(eventTime)
        << eventTime.date().year() << "-" << f2 % eventTime.date().month().as_number() << "-" << f2 % eventTime.date().day().as_number() << " "
        << f2 % eventTime.time_of_day().hours() << ":" << f2 % eventTime.time_of_day().minutes() << ":" << f2 % eventTime.time_of_day().seconds()
        << " [" << system << "] " << level << " " << file << ":" << line << "-" << function << "-> " << message << std::endl;

    if(useSinks)
        sink(system, time, level, lv, file, line, function, message);


/*
 // Examples because my brain don't work
    auto epoch = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::cout << epoch << " " << (long)epoch << std::endl;


    boost::posix_time::ptime const time_epoch(boost::gregorian::date(1970, 1, 1));
    auto ns = (boost::posix_time::microsec_clock::universal_time() - time_epoch).total_nanoseconds();
    std::cout << ns << "\n";

    std::cout << (boost::posix_time::microsec_clock::universal_time() - boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1))).total_nanoseconds() << endl;

    boost::posix_time::ptime now = time_epoch + boost::posix_time::milliseconds(epoch / 1000000);
    std::cout << boost::posix_time::to_iso_string(now);

*/
}
