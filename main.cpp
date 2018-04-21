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
    testparser("x*x");

    Map mymap(10, 10);
    Renderer renderer;
    renderer.setScale(sf::Vector2f(4, 4));
    RobotLoader robotloader;
    FunctorRandomPos posRandomizer(&mymap);
    robotloader.setRobotPoses(posRandomizer);

    sf::RenderWindow fenetre(sf::VideoMode(40*TILE_SIZE, 40*TILE_SIZE), "SFML works!");
    fenetre.setFramerateLimit(144);

    sf::Clock c;
    float dt;

    while (fenetre.isOpen())
    {
        dt = c.restart().asMicroseconds()*0.000001f;

        sf::Event event;
        while (fenetre.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                fenetre.close();
        }

        robotloader.updateScripts(dt);

        //RENDERER DROPPING
        mymap.dropToRenderer(renderer);
        robotloader.dropRobotsToRenderer(renderer);
        robotloader.updateRobotsImg();

        //RENDERER RENDER
        fenetre.clear(sf::Color::White);
        renderer.render(fenetre);
        fenetre.display();
    }

    return 0;
}
