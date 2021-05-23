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
    //boost::asio::io_service &ios;
    bool running;
    bool started;   //set to true on the first cycle called
    boost::asio::deadline_timer timer;
    int fps = 20;
    long last_ms;
    boost::asio::io_service::strand strand;

    //Strand will be owned by the VM that owns the node and lua state machine
    class NodeTester: public vae::vsm::chunk::NodeGrip{
        boost::asio::io_service::strand &strand;
        vae::vsm::chunk::Vector pos;
    public:
        NodeTester(boost::asio::io_service::strand &strand): strand(strand), pos(-1,-1) {
        }

        ///interface:
        boost::asio::io_context::strand& getStrand(){ return strand; }
        void updatePos(vae::vsm::chunk::Vector pos) {
            this->pos = pos;
        };
        void updateMapId(vae::vsm::chunk::MapId id) {
            LOG(Debug) << "Set test node map ID to " << id;
        };
    };

    //Strand will be owned by the VM that owns the viewport and lua state machine
    class ViewportTester: public vae::vsm::chunk::ViewportGrip{
        boost::asio::io_service::strand &strand;
        std::map<vae::vsm::chunk::Node*, vae::vsm::chunk::Node*> nodesInSight;
        vae::vsm::chunk::coordType x, y;
        vae::vsm::chunk::DimType w, h;
    public:
        ViewportTester(boost::asio::io_service::strand &strand): strand(strand) {
        }
        vae::vsm::chunk::coordType getX() const { return x; }
        vae::vsm::chunk::coordType getY() const { return y; }

        void draw(vae::vsm::TestVisualize &v){
            v.drawRect(x, y, w, h, sf::Color::Green);
        }

        ///Interface:
        boost::asio::io_context::strand& getStrand(){ return strand; }
        virtual void updatePos(vae::vsm::chunk::Vector pos) {
        };
        virtual void updateMapId(vae::vsm::chunk::MapId id) {
            LOG(Debug) << "Set test viewport map ID to " << id;
        };
        virtual void updatePos(vae::vsm::chunk::coordType x, vae::vsm::chunk::coordType y, vae::vsm::chunk::DimType width, vae::vsm::chunk::DimType height) {
            this->x = x;
            this->y = y;
            this->w = width;
            this->h = height;
        };

        //A node did something
        virtual void changeNode(size_t typeHash, vae::vsm::chunk::NodeAction::Ptr dis){
        }
        //When the viewpoint is registered, this will add all nodes to it. Or a new node is created
        virtual void addNode(vae::vsm::chunk::Node *node){
        };
        //A node enters the viewpoint from another Chunklet
        virtual void enterNode(vae::vsm::chunk::Node *node){
        };
        //A node exits the viewpoint to another Chunklet
        virtual void exitNode(vae::vsm::chunk::Node *node /* perhaps to Chunklet? */){
        };
    };

    vae::vsm::chunk::Composer &mapComposer;
    NodeTester nodeTester;
    ViewportTester viewportTester;
    vae::vsm::chunk::Node::Ptr node;
    vae::vsm::chunk::Viewpoint *viewport;
    double vd = 0;
    double speed = 1;

    vae::vsm::TestVisualize testVisualize;
    vae::vsm::chunk::MapId mapId;
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
            viewportTester.draw(testVisualize);
            mapComposer.getMap(FIRST_MAP)->draw(testVisualize);
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

    TestService(boost::asio::io_service &ios, sf::RenderWindow *window, vae::vsm::chunk::Composer &mapComposer):
            Service("TestService"),
            //ios(ios),
            strand(ios),
            timer(ios),
            testVisualize(window),
            mapComposer(mapComposer),
            mapId(FIRST_MAP),
            running(true),
            node(nullptr),
            nodeTester(strand),
            viewportTester(strand),
            stressService(ios, mapComposer)
            {
                setTimer();
                node = mapComposer.getMap(FIRST_MAP)->newNode(nodeTester, 40, 40);
                viewport = mapComposer.getMap(FIRST_MAP)->newViewpoint(viewportTester, 30, 30, 25, 25);
                started = false;
            }
    void processKeyPress(sf::Keyboard::Key key){
        LOG(Debug) << "Keypress: " << key;
        switch(key){
            case sf::Keyboard::Key::Down: viewport->setY(viewportTester.getY() + 8); break;
            case sf::Keyboard::Key::Up: viewport->setY(viewportTester.getY() - 8); break;
            case sf::Keyboard::Key::Left: viewport->setX(viewportTester.getX() - 8); break;
            case sf::Keyboard::Key::Right: viewport->setX(viewportTester.getX() + 8); break;

            //case sf::Keyboard::Key::W: node->setPos(node->getX(), node->getY() - 1); break;
            //case sf::Keyboard::Key::A: node->setPos(node->getX() - 1, node->getY()); break;
            //case sf::Keyboard::Key::S: node->setPos(node->getX(), node->getY() + 1); break;
            //case sf::Keyboard::Key::D: node->setPos(node->getX() + 1, node->getY()); break;
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
