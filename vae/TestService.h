//
// Created by protocol on 4/21/21.
//

#ifndef BOOSTTESTING_TESTSERVICE_H
#define BOOSTTESTING_TESTSERVICE_H

#include <boost/bind.hpp>

#include "vsm/Map.h"

class TestService {
    boost::asio::io_service &ios;
    boost::asio::deadline_timer timer;
    int fps = 10;

    vae::vsm::TestVisualize testVisualize;
    vae::vsm::chunk::Composer composer;
    vae::vsm::chunk::Map::Id mapId;
    vae::vsm::chunk::Node node;
    vae::vsm::chunk::Viewport viewport;
public:
    void cycle(){
        testVisualize.cycle();
        node.setX(node.getXNode().getPos() + 1);
        setTimer();
    }
    void setTimer(){
        timer.expires_from_now( boost::posix_time::milliseconds (int(1000.0 * ((float)fps/60.0))));
        timer.async_wait( boost::bind( &TestService::cycle, this));
    }
    TestService(boost::asio::io_service &ios):
            ios(ios),
            timer(ios),
            testVisualize("Vaewyn Server Mapping Visualizer"),
            composer("???"),
            mapId("chunkMap-zero"),
            viewport(2, 3,5, 5)
            {
                setTimer();

                node.setMapId(mapId);
                composer.insert(node);
                composer.insert(viewport, mapId);

                auto beep = [&](sf::Keyboard::Key key){
                    std::cout << key << "\t\t\t";
                    switch(key){
                        case sf::Keyboard::Key::Down: viewport.setY(viewport.getY() + 3); break;
                        case sf::Keyboard::Key::Up: viewport.setY(viewport.getY() - 4); break;
                        case sf::Keyboard::Key::Left: viewport.setX(viewport.getX() - 4); break;
                        case sf::Keyboard::Key::Right: viewport.setX(viewport.getX() + 3); break;

                        case sf::Keyboard::Key::W: node.setY(node.getYNode().getPos() - 1); break;
                        case sf::Keyboard::Key::A: node.setX(node.getXNode().getPos() - 1); break;
                        case sf::Keyboard::Key::S: node.setY(node.getYNode().getPos() + 1); break;
                        case sf::Keyboard::Key::D: node.setX(node.getXNode().getPos() + 1); break;

                            //case sf::Keyboard::Key::Space: node.say("Hello."); break;
                        case sf::Keyboard::Key::Escape: testVisualize.getWindow().close(); ios.stop(); break;
                    }
                };
                testVisualize.regKeyboard(beep);

                auto drawer = [&](){
                    composer.draw(testVisualize);
                };
                testVisualize.regDraw(drawer);
            }
};


#endif //BOOSTTESTING_TESTSERVICE_H
