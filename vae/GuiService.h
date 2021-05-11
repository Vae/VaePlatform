//
// Created by protocol on 5/1/21.
//

#ifndef BOOSTTESTING_GUISERVICE_H
#define BOOSTTESTING_GUISERVICE_H


#include <SFML/Graphics/RenderWindow.hpp>

class GuiService {
    static sf::RenderWindow *window;
    GuiService(){
        if(window == nullptr)
            window = new sf::RenderWindow(sf::VideoMode(800, 600), "Vaewyn Server Mapping Visualizer");
    }
    ~GuiService(){
        delete window;
        window = nullptr;
    }
};


#endif //BOOSTTESTING_GUISERVICE_H
