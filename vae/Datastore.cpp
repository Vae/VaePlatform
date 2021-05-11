//
// Created by protocol on 4/21/21.
//
#include <iostream>
#include <cassert>
#include "Datastore.h"
#include "log.h"
bool vae::Datastore::connect(std::string connectString) {
    assert(conn.get() == 0);
    try{

        /**
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *      We are not allowed to use the logging system during database init!
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         */

        conn.reset(new pqxx::connection(connectString));
        if (conn->is_open()) {
            //LOG(Info) << "Opened database successfully: " << conn->dbname() << ".";
            std::cout << "Opened database successfully: " << conn->dbname() << "." << std::endl;
        } else {
            //LOG(Error) << "Can't open database.";
            std::cout << "Can't open database." << std::endl;
            return true;
        }
        return false;
    } catch (const std::exception &e) {
        //LOG(Error) << e.what();
        std::cout << "Database exception:" << std::endl << e.what() << std::endl;
        return 1;
    }
}