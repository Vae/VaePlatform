#include <iostream>
#include "BoostSerializationTest.h"
#include "PostgreSQLTest.h"

#include "vae/vsm/TestVisualize.h"
#include "vae/vsm/BasicMap.h"

#include "vae/vsm/ChunkMap.h"
#include "olib/concurrentqueue.h"

int main() {
    //std::cout << "BoostSerializationTest: " << BoostSerializationTest::boostSerializationTest() << std::endl;
    //std::cout << "PostgreSQLTest: " << PostgreSQLTest::postgreSQLTest() << std::endl;

    moodycamel::ConcurrentQueue<int> q;
    q.enqueue(25);
    int item;
    bool found = q.try_dequeue(item);
    assert(found && item == 25);

    std::map<int, void*> aav;
    aav[3] = 0;

    vae::vsm::TestVisualize testVisualize;

    vae::vsm::basic::Map bmap(120, 80);

    //Test vae::vsm::chunk::Map functions
    vae::vsm::chunk::ChunkMapComposer mapComposer;
    vae::vsm::chunk::Map::Id mapId("map-zero");
    vae::vsm::chunk::Node node;
    node.setMapId(mapId);
    mapComposer.insert(node);
    vae::vsm::chunk::Viewport viewport(18, 10);
    mapComposer.insert(viewport, mapId);

    //for(int a = 0; a < 100; a++)
    //    node.moveX(node.getXNode().getPos() + 1);

    while(testVisualize.getWindow().isOpen()){
        sf::Event event;
        while (testVisualize.getWindow().pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    testVisualize.getWindow().close();
                    break;
                case sf::Event::KeyPressed:
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){}
                    break;
                default:
                    break;
            }
        }
        //bmap.draw(testVisualize);
        //mapComposer.draw(testVisualize);
        testVisualize.cycle();

        if(node.getXNode().getPos() < 10)
            node.setX(node.getXNode().getPos() + 1);
        else
            node.setX(node.getXNode().getPos() + 3);
        //else
         //   node.moveY(node.getYNode().getPos() + 1);
    }
    testVisualize.getWindow().close();

    std::cout << "Completed all tests." << std::endl;
    return 0;
}

