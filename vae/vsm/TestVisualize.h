//
// Created by protocol on 4/14/21.
//

#ifndef BOOSTTESTING_TESTVISUALIZE_H
#define BOOSTTESTING_TESTVISUALIZE_H

#include<SFML/Graphics.hpp>
#include <boost/noncopyable.hpp>

#include "vae/defs.hpp"
#include "vae/log.h"

namespace vae {

    namespace vsm {

        class TestVisualize: private boost::noncopyable {
            bool doDraw, doKeyboard, doCycle;
        public:
            sf::RenderWindow *window;
            sf::Font font;
        public:
            ~TestVisualize(){
                doDraw = false;
                doKeyboard = false;
                doCycle = false;
                window = nullptr;
            }
            sf::RenderWindow& getWindow() { return *window; }
            sf::RenderWindow* getWindowPtr() { return window; }
            TestVisualize(sf::RenderWindow *window): window(window) {

                window->clear(sf::Color::Blue);
                if (!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMono.ttf")) {
                    std::cerr << "Failed to load font FreeMono.ttf" << std::endl;
                }
                doDraw = false;
                doKeyboard = false;
                doCycle = false;
            }

            void drawText(int x, int y, std::string str, sf::Color col = sf::Color::White) {
                sf::Text text;

                // select the font
                text.setFont(font); // font is a sf::Font

                //text.setPosition(x, y);

                // set the string to display
                text.setString(str);

                // set the character size
                text.setCharacterSize(24); // in pixels, not points!
                text.setFillColor(col);

                // set the color
                // text.setColor(sf::Color::White);

                text.setStyle(sf::Text::Bold | sf::Text::Underlined);
                // inside the main loop, between window.clear() and window.display()
                window->draw(text);
            }

            void drawPoint(int x, int y, sf::Color col = sf::Color::White){
                sf::RectangleShape rect(sf::Vector2f(1, 1));
                rect.setPosition(x, y);
                rect.setFillColor(col);
                window->draw(rect);
            }

            void drawLine(int x1, int y1, int x2, int y2, sf::Color col = sf::Color::White) {
                sf::Vertex line[] = {
                                        sf::Vertex(sf::Vector2f(x1, y1)),
                                        sf::Vertex(sf::Vector2f(x2, y2))
                                };
                window->draw(line , 2, sf::Lines);
            }

            void drawRect(int x, int y, int w, int h, sf::Color col = sf::Color::White) {
                sf::RectangleShape rect(sf::Vector2f(w, h));
                rect.setPosition(x, y);
                rect.setOutlineColor(col);
                rect.setOutlineThickness(1);
                rect.setFillColor(sf::Color::Transparent);
                window->draw(rect);
            }
            std::function<void(sf::Keyboard::Key)> keyboardFunction;
            void regKeyboard(std::function<void(sf::Keyboard::Key)> f){
                this->keyboardFunction = f;
                doKeyboard = true;
            }

            std::function<void()> drawFunction;
            void regDraw(std::function<void()> f){
                this->drawFunction = f;
                doDraw = true;
            }
            std::function<void()> cycleFunction;
            void regCycle(std::function<void()> f){
                this->cycleFunction = f;
                doCycle = true;
            }
        };

    }   //namespace vsm

}   //namespace vae


#endif //BOOSTTESTING_TESTVISUALIZE_H
