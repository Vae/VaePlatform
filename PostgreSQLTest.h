//
// Created by protocol on 4/13/21.
//

#ifndef BOOSTTESTING_POSTGRESQLTEST_H
#define BOOSTTESTING_POSTGRESQLTEST_H

#include <iostream>
#include <pqxx/pqxx>

class PostgreSQLTest {
public:
    static int postgreSQLTest(){
        using namespace std;
        using namespace pqxx;
        try {
            connection C("dbname = test1 user = root password = pass hostaddr = 192.168.99.10 port = 25432");
            if (C.is_open()) {
                cout << "Opened database successfully: " << C.dbname() << endl;
            } else {
                cout << "Can't open database" << endl;
                return 1;
            }
            C.disconnect ();
        } catch (const std::exception &e) {
            cerr << e.what() << std::endl;
            return 1;
        }
        return 0;
    }
};


#endif //BOOSTTESTING_POSTGRESQLTEST_H
