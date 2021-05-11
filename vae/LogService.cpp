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
void LogService::cout_impl(const char* system, long time, const char* level, const float lv, const char* file, const int line, const char* function, const std::string message){
    boost::format f2("%02u");
    boost::posix_time::ptime eventTime = boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1)) + boost::posix_time::milliseconds(time / 1000000);
    //https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
/*
         foreground background
black        30         40
red          31         41
green        32         42
yellow       33         43
blue         34         44
magenta      35         45
cyan         36         46
white        37         47
 */

    //cout << "\033[1;31mbold red text\033[0m\n";
    const char* consoleColor = "";
    if(lv >= 10) consoleColor =    "\033[0;37m";  //info
    if(lv >= 100) consoleColor =    "\033[1;33m"; //warning
    if(lv >= 1000) consoleColor =   "\033[1;31m"; //error
    if(lv >= 10000) consoleColor =  "\033[6;31;43m"; //security

    std::cout
            //<< boost::posix_time::to_iso_string(eventTime)
            << "\033[0m"  //reset colors
            << eventTime.date().year() << "-" << f2 % eventTime.date().month().as_number() << "-"
            << f2 % eventTime.date().day().as_number() << " "
            << f2 % eventTime.time_of_day().hours() << ":" << f2 % eventTime.time_of_day().minutes() << ":"
            << f2 % eventTime.time_of_day().seconds()
            << " [" << system << "] " << consoleColor << level << "\033[0m " << file << ":" << line << "-" << function << "-> "
            << message << std::endl;
}
void LogService::log_impl(const char* system, long time, const char* level, const float lv, const char* file, const int line, const char* function, const std::string &message){
    {
        boost::mutex::scoped_lock lock(logEntryHx.mutex);
        logEntryHx[level][system][file][line]++;
        std::shared_ptr<std::vector<int>> p(new std::vector<int>);

        if(logEntryHx[level][system][file][line] < 25)
            coutStrand.post(boost::bind(&LogService::cout_impl, this, system, time, level, lv, file, line, function, message));
    }
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
