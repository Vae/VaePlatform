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

    vae::vsm::chunk::Map cmap(8);
    cmap.pregenChunks(0, 0, 8);

    int x = 0, y = 0;
    while(testVisualize.getWindow().isOpen()){
        bmap.draw(testVisualize);
        testVisualize.cycle();
    }

    std::cout << "Completed all tests." << std::endl;
    return 0;
}
