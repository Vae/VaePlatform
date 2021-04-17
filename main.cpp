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
    vae::vsm::chunk::Map::Ptr cmap = mapComposer.getMap(mapId);

    vae::vsm::chunk::Node node;
    cmap->insert(node);

    vae::vsm::chunk::Viewport vp;
    cmap->insert(vp);


    int x = 0, y = 0;
    while(testVisualize.getWindow().isOpen()){
        //bmap.draw(testVisualize);
        cmap->draw(testVisualize);
        testVisualize.cycle();
    }

    std::cout << "Completed all tests." << std::endl;
    return 0;
}
