//
// Created by protocol on 4/21/21.
//

#ifndef BOOSTTESTING_DATASTORE_H
#define BOOSTTESTING_DATASTORE_H

#include <pqxx/pqxx>
#include "log.h"

class Datastore {
    std::shared_ptr<pqxx::connection> conn;

    public:
    //Datastore(std::string connectionString){}
    bool connect(std::string connectString) {
        assert(conn.get() == 0);
        try{
            conn.reset(new pqxx::connection(connectString));
            if (conn->is_open()) {
                LOG(Info) << "Opened database successfully: " << conn->dbname();
            } else {
                LOG(Error) << "Can't open database";
                return true;
            }
            return false;
        } catch (const std::exception &e) {
            LOG(Error) << e.what();
            return 1;
        }
    }
};


#endif //BOOSTTESTING_DATASTORE_H
