//
// Created by protocol on 4/21/21.
//

#ifndef BOOSTTESTING_LOGSTORE_H
#define BOOSTTESTING_LOGSTORE_H


#include <boost/asio/io_service.hpp>

template<typename InfluxdbType>
class LogStore {
public:
    LogStore(){
    }
    bool connect(){
        return false;
    }
};


#endif //BOOSTTESTING_LOGSTORE_H
