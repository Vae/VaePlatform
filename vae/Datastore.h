//
// Created by protocol on 4/21/21.
//

#ifndef BOOSTTESTING_DATASTORE_H
#define BOOSTTESTING_DATASTORE_H

#include <pqxx/pqxx>
//#include "log.h"

class Datastore {
    std::shared_ptr<pqxx::connection> conn;

    public:
    //Datastore(std::string connectionString){}
    bool connect(std::string connectString);
};


#endif //BOOSTTESTING_DATASTORE_H
