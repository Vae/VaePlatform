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

            bool running;

            int maxDist = 0;
            int dist = 0;
            VectorDirectionType vd;
            SpeedType speed;

            boost::asio::deadline_timer timer;

            /*
            void setTimer() {
                if (running) {
                    timer.expires_from_now(boost::posix_time::milliseconds(speed));
                    timer.async_wait(boost::bind(&NodeController::cycle, this));
                }
            }
            */

        public:
            NodeController(boost::asio::io_service &ios, std::string name) : ios(ios), timer(ios), name(name), running(true) {
                maxDist = rand() % 1000;
                speed = randomDouble(0.2, 8);
                maxDist = randomDouble(80, 800);
                vd = randomDouble(0, PI * 2);
                //setTimer();
            }
            ~NodeController(){
                timer.cancel();
            }
            void cycle(double delta_t) {
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

                node.movePos(vd, speed * delta_t);

                //setTimer();
            }
            vae::vsm::chunk::Node &getNode() { return node; }
        };

        class StressService {
            boost::asio::io_service &ios;
            vae::vsm::chunk::Composer &composer;
            std::list<NodeController *> nodeControllers;
        public:
            StressService(boost::asio::io_service &ios, vae::vsm::chunk::Composer &composer) : ios(ios), composer(composer) {
            }
            ~StressService(){
                for(auto i : nodeControllers) {
                    delete i;
                }
            }
            void cycle(double delta_t) {
                for(auto i : nodeControllers)
                    i->cycle(delta_t);
            }

            void newNode(vae::vsm::chunk::Map::Id mapId, std::string name) {
                nodeControllers.emplace_back(new NodeController(ios, name));
                nodeControllers.back()->getNode().setMapId(mapId);
                composer.insert(nodeControllers.back()->getNode(), rand() %800, rand()%600);
            }
        };
    }   //namespace test
}   //namespace vae

#endif //BOOSTTESTING_STRESSSERVICE_H
