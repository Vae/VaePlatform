//
// Created by protocol on 4/20/21.
//

#include "log.h"

vl::LogEngine logInstance("global");

std::string vl::FormatTime(boost::posix_time::ptime now){
    using namespace boost::posix_time;
    static std::locale loc(std::wcout.getloc(),
                           new wtime_facet(L"%Y%m%d _ %H%M%S"));
    std::ostringstream wss;
    wss.imbue(loc);
    wss << now;
    return wss.str();
}
inline std::string vl::NowTime(){
    return FormatTime(boost::posix_time::second_clock::universal_time());
}

vl::LogLevel::~LogLevel(){
    if(!sent)
        finalize();
}
void vl::LogLevel::finalize(){
    assert(sent == false);
    sent = true;
    logEngine.insert(time, name, level, file, line, functionName, os.str());
}
std::ostringstream &vl::LogLevel::start(const char *file, const int line, const char *function){
    assert(sent == false);
    this->file = (char *)file;
    this->line = line;
    this->functionName = (char *)function;
    return os;
}

std::ostringstream &vl::LogLevel::start(){
    assert(sent == false);
    os << NowTime();
    os << " [" << logEngine.systemName << "] " << name << ": ";
    return os;
}


vl::LogEngine::LogEngine(const char *systemName, std::function<void(const char* system, long time, const char* level, const float lv, const char* file, const int line, const char* function, const std::string &message)> f): file(0), line(0), systemName(systemName), inserter(f){
    useInserter = true;
}
vl::LogEngine::LogEngine(const char *systemName): file(0), line(0), systemName(systemName){
    useInserter = false;
}
void vl::LogEngine::insert(long time, const char* level, const float lv, const char* file, const int line, const char* function, const std::string &dis){
    if(useInserter)
        inserter(this->systemName, time, level, lv, file, line, function, dis);
    else
        std::cout << dis << std::endl;
}

vl::LogEngine::~LogEngine()
{
    if (file)
        os << " (" << file << ":" << line << ")";
//    os << std::endl;
    //T::Output(os.str());
}
