//
// Created by protocol on 4/27/21.
//

#ifndef BOOSTTESTING_STRESSSERVICE_H
#define BOOSTTESTING_STRESSSERVICE_H


#include <boost/asio/io_service.hpp>
#include "vsm/Map.h"
#include "test.h"
#include "vsm/SkipMap.h"

namespace vae {
    namespace test {

        class NodeController {
            boost::asio::io_service &ios;
            std::string name;
        vae::vsm::chunk::Node node;
            typedef std::shared_ptr<NodeController> Ptr;

            const SpeedType moveMaxDelta = 1;

            bool running = true;
            int commandRate = 100 + (rand() % 100);  //in milliseconds, how often should we run a command?

            int maxDist = 0;
            int dist = 0;
            VectorDirectionType vd;
            SpeedType speed;

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
            NodeController(boost::asio::io_service &ios, std::string name) : ios(ios), timer(ios), name(name), running(true) {
                maxDist = rand() % 1000;
                speed = randomDouble(0.2, 8);
                maxDist = randomDouble(80, 800);
                vd = randomDouble(0, PI * 2);
                setTimer();
            }
            ~NodeController(){
                timer.cancel();
            }
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

                node.getChunklet()->strand.post(boost::bind(&vae::vsm::chunk::Node::movePos, &node, vd, speed * delta_t));

                //node.movePos(vd, speed * delta_t);
                if(running)
                    setTimer();
            }
            vae::vsm::chunk::Node &getNode() { return node; }
        };

        class StressService {
            boost::asio::io_service &ios;
            boost::asio::io_context::strand strand;
            vae::vsm::chunk::Composer &composer;
            std::list<NodeController *> nodeControllers;
            void _newNodeStart(NodeController *dis){

            }
            void _newNode(vae::vsm::chunk::Map::Id mapId, std::string name){
                nodeControllers.emplace_back(new NodeController(ios, name));
                nodeControllers.back()->getNode().setMapId(mapId);
                composer.insert(nodeControllers.back()->getNode(), rand() %800, rand()%600, boost::bind<void>(&StressService::_newNodeStart, this, nodeControllers.back()));
            }
        public:
            StressService(boost::asio::io_service &ios, vae::vsm::chunk::Composer &composer) : ios(ios), strand(ios), composer(composer) {
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

            void newNode(vae::vsm::chunk::Map::Id mapId, std::string name) {
                strand.post(boost::bind(&vae::test::StressService::_newNode, this, mapId, name));
            }
        };
    }   //namespace test
}   //namespace vae

#endif //BOOSTTESTING_STRESSSERVICE_H
