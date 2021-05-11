//
// Created by protocol on 5/11/21.
//

#ifndef BOOSTTESTING_ENTITYTESTSERVICE_H
#define BOOSTTESTING_ENTITYTESTSERVICE_H

#include "vae/defs.hpp"
#include "vae/Service.h"
#include "vae/entity/Entity.h"
#include "vae/react/EntityActions.h"
#include "vae/react/EntityAbilities.h"

namespace vae {

    class EntityTestService : public vae::Service {
        vae::EntityComposer ec;
    public:
        EntityTestService(boost::asio::io_service &ios, vae::EntityComposer &ec) :
                Service("TestService"),
                ec(ec) {
        }

        void doTest() {
            vae::Entity::Ptr e = ec.getEntityByName("test");
            if (e) {
                vae::react::Ability::Ptr hearing(new vae::react::AbilityHear);
                e->addNewAbility(hearing);
                //abilityHear
                e->getLua().do_string("abilityHear:hear(\"hey\")");
            } else {
                LOG(Error) << "Didn't get test entity";
            }
        }
    };
}

#endif //BOOSTTESTING_ENTITYTESTSERVICE_H
