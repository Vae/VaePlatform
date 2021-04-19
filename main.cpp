#include "defs.h"

#include <iostream>
#include "BoostSerializationTest.h"
#include "PostgreSQLTest.h"

#include "vae/vsm/TestVisualize.h"
#include "vae/vsm/BasicMap.h"

#include "vae/vsm/Map.h"
#include "olib/concurrentqueue.h"

int main() {
    //std::cout << "BoostSerializationTest: " << BoostSerializationTest::boostSerializationTest() << std::endl;
    //std::cout << "PostgreSQLTest: " << PostgreSQLTest::postgreSQLTest() << std::endl;

    /*
    moodycamel::ConcurrentQueue<int> q;
    q.enqueue(25);
    int item;
    bool found = q.try_dequeue(item);
    assert(found && item == 25);
     */


    vae::vsm::TestVisualize testVisualize("Vaewyn Server Mapping Visualizer");

    //Test vae::vsm::chunk::Map functions
    vae::vsm::chunk::Composer composer("???");
    vae::vsm::chunk::Map::Id mapId("map-zero");
    vae::vsm::chunk::Node node;
    node.setMapId(mapId);
    composer.insert(node);
    vae::vsm::chunk::Viewport viewport(2, 3,5, 5);
    composer.insert(viewport, mapId);

    auto beep = [&](sf::Keyboard::Key key)  {
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
            case sf::Keyboard::Key::Escape: testVisualize.getWindow().close();
        }
    };
    testVisualize.regKeyboard(beep);

    auto drawer = [&](){
        composer.draw(testVisualize);
    };
    testVisualize.regDraw(drawer);

    while(testVisualize.getWindow().isOpen()){
        testVisualize.cycle();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        node.setX(node.getXNode().getPos() + 1);
    }

    std::cout << "Completed all tests." << std::endl;
    return 0;
}
