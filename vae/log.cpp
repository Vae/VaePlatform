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
    logEngine.insert(os.str());
}
std::ostringstream &vl::LogLevel::start(const char *file, const int line){
    assert(sent == false);
    os << NowTime();
    if(codeDetails)
        os << " [" << logEngine.systemName << "] " << name << " " << file << ":" << line << "-> ";
    else
        os << " [" << logEngine.systemName << "] " << name << ": ";
    return os;
}
std::ostringstream &vl::LogLevel::start(){
    assert(sent == false);
    os << NowTime();
    os << " [" << logEngine.systemName << "] " << name << ": ";
    return os;
}

vl::LogEngine::LogEngine(const char *systemName) : file(0), line(0), systemName(systemName){
}
vl::LogEngine::LogEngine(const char * systemName, std::function<void(const std::string&)> f): file(0), line(0), systemName(systemName), inserter(f){
}

void vl::LogEngine::insert(const std::string &dis){
    if(useInserter)
        inserter(dis);
    else
        defaultInsert(dis);
}

std::ostringstream& vl::LogEngine::get(LogLevel level){
    os << NowTime();
    os << " " << level.name << ": ";
//    os << std::string(level > logDEBUG ? level - logDEBUG : 0, '\t');
    return os;
}

vl::LogEngine::~LogEngine()
{
    if (file)
        os << " (" << file << ":" << line << ")";
//    os << std::endl;
    //T::Output(os.str());
}
