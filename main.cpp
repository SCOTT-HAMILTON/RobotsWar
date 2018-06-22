#include <iostream>
#include <SFML/Graphics.hpp>

#include "Map.h"
#include "Renderer.h"
#include "RobotLoader.h"

class FunctorRandomPos{
public:
    FunctorRandomPos(Map *_map) :
        _map(_map)
    {
    }
    sf::Vector2f operator()(){
        if (_map != nullptr)return _map->getRandomFreeCase();
        return sf::Vector2f(0, 0);
    }
private:
    Map *_map;
};

void testparser(const std::string &expr){
}

int main()
{
    Robot::initTexts();

    testparser("x*x");
    Map mymap(18, 18);
    Renderer renderer;
    renderer.setScale(sf::Vector2f(2, 2));
    FunctorRandomPos posRandomizer(&mymap);
    RobotLoader robotloader(mymap, posRandomizer);
    sf::RenderWindow fenetre(sf::VideoMode(mymap.getNbTilesWidth()*TILE_SIZE*renderer.getScale().x, mymap.getNbTilesHeight()*TILE_SIZE*renderer.getScale().y), "RobotsWar !!!");
    fenetre.setFramerateLimit(60);

    sf::Font font;font.loadFromFile("res/absender1.ttf");
    sf::Text frameratestext("0 FPS", font, 20);
    frameratestext.setPosition(10, 10);
    frameratestext.setFillColor(sf::Color::Black);

    sf::Clock c;
    sf::Clock fpsupdate;
    float dt;
    sf::Clock timer;
    bool pause = false;

    unsigned long framecounter = 0;


    sf::RectangleShape black_square(sf::Vector2f(16*renderer.getScale().x, 16*renderer.getScale().y));
    black_square.setFillColor(sf::Color(0, 0, 0));

    while (fenetre.isOpen())
    {
        framecounter++;
        double clockt = c.restart().asMicroseconds();
        dt = clockt*0.000001f;

        if (fpsupdate.getElapsedTime().asMilliseconds()>500){
            fpsupdate.restart();
            frameratestext.setString(std::to_string( framecounter*2 )+" FPS");
            framecounter = 0;
        }
        sf::Event event;
        while (fenetre.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                fenetre.close();
            else if (event.type == sf::Event::KeyPressed){
                if (event.key.code == sf::Keyboard::P)pause = !pause;
            }
        }

        if (!pause)robotloader.updateScripts(dt, mymap);
        sf::Vector2f pos = robotloader.getRobot(0).getPos();
        black_square.setPosition(pos.x*renderer.getScale().x, pos.y*renderer.getScale().y);

        //RENDERER DROPPING
        mymap.dropToRenderer(renderer);
        robotloader.dropRobotsToRenderer(renderer);
        robotloader.updateRobotsImg();

        //RENDERER RENDER
        fenetre.clear(sf::Color::White);
        renderer.render(fenetre);
        fenetre.draw(frameratestext);
        fenetre.display();
    }

    return 0;
}
