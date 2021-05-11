//
// Created by protocol on 4/21/21.
//

#ifndef BOOSTTESTING_DATASTORE_H
#define BOOSTTESTING_DATASTORE_H

#include <pqxx/pqxx>
#include <boost/noncopyable.hpp>

namespace vae {
    class Datastore : public boost::noncopyable {
        std::shared_ptr<pqxx::connection> conn;
    public:
        typedef std::shared_ptr<vae::Datastore> Ptr;
        //Maybe...
        //class Transaction{
        //    typedef std::shared_ptr<Transaction> Ptr;
        //};
        //Datastore(std::string connectionString){}
        //Datastore(std::string a){ std::cout << a; }
        bool connect(std::string connectString);

        //std::shared_ptr<pqxx::connection> getContext() { return conn; }
        pqxx::connection &getContext() { return *conn; }
        //Transaction newTransaction(){
        //    return
        //}
    };
}   //namespace vae

#endif //BOOSTTESTING_DATASTORE_H
