//
// Created by protocol on 4/29/21.
//

#ifndef BOOSTTESTING_ENTITY_H
#define BOOSTTESTING_ENTITY_H

#include <string>

#include "vae/vsm/Map.h"
#include "vae/Datastore.h"
#include "LuaState.h"
#include "vae/react/Ability.h"

namespace vae{

    //class Viewport: public vae::vsm::chunk::Viewpoint{    };

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
    public:
        typedef std::shared_ptr<Entity> Ptr;
        typedef vae::react::Ability Ability;
    private:
        EntityID eID;
        boost::asio::io_context::strand strand;
        vae::vsm::chunk::Node node;
        LuaState luaState;
        std::map<std::string, Ability::Ptr> abilities;
    public:
        Entity(boost::asio::io_context &ios): strand(ios){
        }
        void addNewAbility(Ability::Ptr dis){
            if(abilities.count(dis->getName())) {
                LOG(Warn) << "Ability already present.";
                return;
            }
            else{
                abilities[dis->getName()] = dis;
                dis->enlist(luaState);
            }
        }
        //LuaState &getLuaState(){ return luaState; }
        sol::state &getLua() { return luaState.getLua(); }
    };

    class AbilitySee{

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
                const char* sql = "SELECT value FROM vs.EntityComposer WHERE key = 'nextEntityId';";

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

        Entity::Ptr getEntityByName(std::string name){
            //TODO: Interface this fetching with the database, loading all items needed to construct an entity.
            //For now, do it all manually
            Entity::Ptr p(new Entity(ios));
            return p;
        }

    };
}   //namespace vae
#endif //BOOSTTESTING_ENTITY_H
