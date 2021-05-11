//
// Created by protocol on 4/29/21.
//

#ifndef BOOSTTESTING_HEREGOES_H
#define BOOSTTESTING_HEREGOES_H

#include "vsm/Map.h"
#include "Datastore.h"
class EntityXComposer {
    //boost::asio::io_service &ios;
    vae::Datastore::Ptr datastore;
    //EntityXComposer(){}
public:
    EntityXComposer(std::string test, vae::Datastore::Ptr datastore): datastore(datastore){
    }
    bool start(){
        return false;
    }
};


#endif //BOOSTTESTING_HEREGOES_H
