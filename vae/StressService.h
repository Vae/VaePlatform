//
// Created by protocol on 4/27/21.
//

#ifndef BOOSTTESTING_STRESSSERVICE_H
#define BOOSTTESTING_STRESSSERVICE_H


#include <boost/asio/io_service.hpp>
#include "vsm/Map.h"
#include "test.h"
#include "vsm/SkipMap.h"

namespace vae::test {
    class NodeController: public vae::vsm::chunk::NodeGrip, private boost::noncopyable {
        boost::asio::io_context::strand strand;
        std::string name;
        vae::vsm::chunk::Node::Ptr node;
        vae::vsm::chunk::Composer &mapComposer;

        typedef std::shared_ptr<NodeController> Ptr;

        const SpeedType moveMaxDelta = 1;

        bool running = true;
        int commandRate = 100 + (rand() % 100);  //in milliseconds, how often should we run a command?

        int maxDist = 0;
        int dist = 0;
        VectorDirectionType vd;
        SpeedType speed;

        vae::vsm::chunk::Vector pos;

        boost::asio::deadline_timer timer;

        bool started = false;   //set to true on the first cycle called
        long last_ms = 0;

        void setTimer(){
            if(running) {
                timer.expires_from_now(boost::posix_time::milliseconds(commandRate));
                timer.async_wait(boost::bind(&NodeController::cycle, this));
            }
        }
    public:
        NodeController(boost::asio::io_service &ios, std::string name, vae::vsm::chunk::Composer &mapComposer):
        strand(ios),
        timer(ios),
        name(name),
        mapComposer(mapComposer),
        running(true),
        node(nullptr),
        pos(-1, -1){
            maxDist = rand() % 1000;
            speed = randomDouble(0.2, 8);
            maxDist = randomDouble(80, 800);
            vd = randomDouble(0, PI * 2);

            vae::vsm::chunk::Map::Ptr map = mapComposer.getMap(FIRST_MAP);
            node = map->newNode(*this, rand() %800, rand()%600);
        }
        ~NodeController(){
            timer.cancel();
        }
        boost::asio::io_context::strand& getStrand(){ return strand; }
        virtual void updatePos(vae::vsm::chunk::Vector pos) {
            this->pos = pos;
        };
        virtual void updateMapId(vae::vsm::chunk::MapId id) {
            //once we get an id, it means we're on a valid map
            setTimer();
        };
        void cycle() {
            vae::TimeType delta_t;
            static boost::posix_time::ptime const epoch(boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1)));
            long current_ms = (boost::posix_time::microsec_clock::universal_time() - epoch).total_milliseconds();
            if(started == false){
                delta_t = 1;
                last_ms = current_ms;
                started = true;
            }
            else {
                delta_t = (double) (current_ms - last_ms) / 1000;
                last_ms = current_ms;
                LOG(Debug) << "Delta_t: " << delta_t;
            }

            //boost::this_thread::sleep(boost::posix_time::milliseconds(1000));


            if (dist >= maxDist) {
                vd = randomDouble(0, PI * 2);
                dist = 0;
            }

            dist += speed;
            /** //This is not built into Node
            for(SpeedType delta_move = speed; delta_move > 0; delta_move -= moveMaxDelta) {
                SpeedType move = (delta_move > moveMaxDelta) ? moveMaxDelta : delta_move;
                vae::vsm::chunk::coordType newX = node.getX() + (cos(vd) * move * delta_t);
                vae::vsm::chunk::coordType newY = node.getY() + (sin(vd) * move * delta_t);

                //node.setX(node.getX() + 1);
                node.setX(newX);
                node.setY(newY);
            }*/

            node->movePos(vd, speed * delta_t);
            //node.getChunklet()->strandMovement.post(boost::bind(&vae::vsm::chunk::Node::movePos, &node, vd, speed * delta_t));

            //node.movePos(vd, speed * delta_t);
            if(running)
                setTimer();
        }
        vae::vsm::chunk::Node *getNode() { return node; }
    };

    class StressService: private boost::noncopyable {
        boost::asio::io_service &ios;
        boost::asio::io_context::strand strand;
        vae::vsm::chunk::Composer &mapComposer;
        std::list<NodeController *> nodeControllers;

        void _newNode(vae::vsm::chunk::MapId mapId, std::string name) {
            nodeControllers.emplace_back(new NodeController(ios, name, mapComposer));
        }
    public:
        StressService(boost::asio::io_service &ios, vae::vsm::chunk::Composer &mapComposer) : ios(ios), strand(ios), mapComposer(mapComposer) {
        }
        ~StressService(){
            for(auto i : nodeControllers) {
                delete i;
            }
        }
        //void cycle(double delta_t) {
        //    for(auto i : nodeControllers)
        //        i->cycle(delta_t);
        //}

        void newNode(vae::vsm::chunk::MapId mapId, std::string name) {
            strand.post(boost::bind(&vae::test::StressService::_newNode, this, mapId, name));
        }
    };
}   //namespace vae::test

#endif //BOOSTTESTING_STRESSSERVICE_H
