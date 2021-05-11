//
// Created by protocol on 4/29/21.
//

#ifndef BOOSTTESTING_ENTITY_H
#define BOOSTTESTING_ENTITY_H

#include <string>

#include "vsm/Map.h"
#include "Datastore.h"

namespace vae{

/**
 * The Entity is the start of what can be called a monster, player, trigger/script, NPC, items etc.
 * Entities:
 *   cannot see: they do not have a viewport.
 *   are part of the collision detection system if they choose to be.
 *   have a visible representation if they choose.
 *   are persistent if they choose to be.
 *   must have a unique ID
 *   are constructed via EntityComposer
 */
    class Entity {
        boost::asio::io_context::strand strand;
        vae::vsm::chunk::Node myNode;
    public:
        Entity(boost::asio::io_context &ios): strand(ios){
        }
        void say(std::string dis){
            //ensure we're on a map
            if(myNode.getChunklet()){

            }
        }
    };

    class EntityComposer{
        boost::asio::io_service &ios;
        vae::Datastore::Ptr ds;
        long nextEntityId;
    public:
        EntityComposer(boost::asio::io_service &ios, vae::Datastore::Ptr ds): ios(ios), ds(ds) {}
        bool start(){
            try{
                /* Create SQL statement */
                char* sql = "SELECT value FROM vs.EntityComposer WHERE key = 'nextEntityId';";

                /* Create a non-transactional object. */
                pqxx::nontransaction N(ds->getContext());

                /* Execute SQL query */
                pqxx::result R( N.exec( sql ));

                /* List down all the records */
                for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) {
                    std::string s = c[0].as<std::string>();
                    nextEntityId = atol(s.c_str());
                }
                LOG(Info) << "Grabbed config successfully (" << nextEntityId << ").";
            } catch (const std::exception &e) {
                LOG(Error) << e.what();
                return true;
            }
            return false;
        }
    };
}   //namespace vae
#endif //BOOSTTESTING_ENTITY_H
