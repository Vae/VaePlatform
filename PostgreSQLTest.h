//
// Created by protocol on 4/13/21.
//

#ifndef BOOSTTESTING_POSTGRESQLTEST_H
#define BOOSTTESTING_POSTGRESQLTEST_H

#include "vae/log.h"
#include <pqxx/pqxx>

class PostgreSQLTest {
public:
    static int postgreSQLTest(){
        using namespace std;
        using namespace pqxx;
        try {
            connection C("dbname = test1 user = root password = pass hostaddr = 192.168.99.10 port = 25432");
            if (C.is_open()) {
                LOG(Info) << "Opened database successfully: " << C.dbname();
            } else {
                LOG(Error) << "Can't open database";
                return 1;
            }
            C.disconnect();
        } catch (const std::exception &e) {
            LOG(Error) << e.what();
            return 1;
        }
        return 0;
    }
};


#endif //BOOSTTESTING_POSTGRESQLTEST_H
