//
// Created by protocol on 4/21/21.
//

#ifndef BOOSTTESTING_TESTSERVICE_H
#define BOOSTTESTING_TESTSERVICE_H


#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind/bind.hpp>

#include "vsm/Map.h"
#include "StressService.h"
#include "test.h"
#include "defs.hpp"
#include "Service.h"

class TestService: public vae::Service {
    boost::asio::io_service &ios;
    bool running;
    bool started;   //set to true on the first cycle called
    boost::asio::deadline_timer timer;
    int fps = 20;
    long last_ms;

    vae::vsm::chunk::Node node;
    double vd = 0;
    double speed = 1;

    vae::vsm::TestVisualize testVisualize;
    vae::vsm::chunk::Composer composer;
    vae::vsm::chunk::Map::Id mapId;
    vae::vsm::chunk::Viewpoint viewport;
    vae::test::StressService stressService;

    void cycle(){
        //For now, compute our own delta-time:
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

        //processGUIEvents must come before (running) check to ensure we don't try to draw after a closing-event is triggered.
        processGUIEvents();
        //same goes for processGameLogic. NOTHING in game logic should be drawing. Naughty naughty
        processGameLogic(delta_t);
        //stressService.cycle(delta_t);

        if(running) {
            composer.draw(testVisualize);
            testVisualize.getWindow().display();
            testVisualize.getWindow().clear(sf::Color::Black);

            setTimer();
        }
    }
    void processGameLogic(double delta_t);
public:
    void setTimer(){
        if(running) {
            timer.expires_from_now(boost::posix_time::milliseconds(int(1000/fps)));
            timer.async_wait(boost::bind(&TestService::cycle, this));
        }
    }

    TestService(boost::asio::io_service &ios, sf::RenderWindow *window):
            Service("TestService"),
            ios(ios),
            timer(ios),
            testVisualize(window),
            composer(ios, "???"),
            mapId("chunkMap-zero"),
            viewport(2, 3,5, 5),
            running(true),
            stressService(ios, composer)
            {
                setTimer();

                node.setMapId(mapId);
                composer.insert(node, 40, 40);
                composer.insert(viewport, mapId);
                started = false;
            }
    void processKeyPress(sf::Keyboard::Key key){
        LOG(Debug) << "Keypress: " << key;
        switch(key){
            case sf::Keyboard::Key::Down: viewport.setY(viewport.getY() + 3); break;
            case sf::Keyboard::Key::Up: viewport.setY(viewport.getY() - 4); break;
            case sf::Keyboard::Key::Left: viewport.setX(viewport.getX() - 4); break;
            case sf::Keyboard::Key::Right: viewport.setX(viewport.getX() + 3); break;

            case sf::Keyboard::Key::W: node.setPos(node.getX(), node.getY() - 1); break;
            case sf::Keyboard::Key::A: node.setPos(node.getX() - 1, node.getY()); break;
            case sf::Keyboard::Key::S: node.setPos(node.getX(), node.getY() + 1); break;
            case sf::Keyboard::Key::D: node.setPos(node.getX() + 1, node.getY()); break;
            case sf::Keyboard::Key::X: vd = vd + 0.2; break;

            case sf::Keyboard::Key::Subtract:
                for(int a = 0; a < 10000; ++a)
                    stressService.newNode(mapId, vae::test::randString(5) + "-" + vae::test::randString(5));
                break;
            case sf::Keyboard::Key::Add:
                stressService.newNode(mapId, vae::test::randString(5) + "-" + vae::test::randString(5));
                break;

                //case sf::Keyboard::Key::Space: node.say("Hello."); break;
            case sf::Keyboard::Key::Escape:
                LOG(Info) << "Stop process.";
                running = false;
                delete testVisualize.getWindowPtr();
                break;
        }
    }
    void processGUIEvents() {
        if (testVisualize.getWindow().isOpen()) {
            sf::Event event;
            while (running && testVisualize.getWindow().pollEvent(event)) {
                switch (event.type) {
                    case sf::Event::Closed:
                        running = false;
                        delete testVisualize.getWindowPtr();
                        break;
                    case sf::Event::KeyPressed:
                            processKeyPress(event.key.code);
                        break;
                    default:
                        break;
                }
            }
        }
    }
};


#endif //BOOSTTESTING_TESTSERVICE_H
