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

    Map mymap(18, 18);
    Renderer renderer;
    renderer.setScale(sf::Vector2f(2, 2));
    RobotLoader robotloader;
    FunctorRandomPos posRandomizer(&mymap);
    robotloader.setRobotPoses(posRandomizer);

    sf::RenderWindow fenetre(sf::VideoMode(mymap.getNbTilesWidth()*TILE_SIZE*renderer.getScale().x, mymap.getNbTilesHeight()*TILE_SIZE*renderer.getScale().y), "RobotsWar !!!");
    fenetre.setFramerateLimit(144);

    sf::Clock c;
    float dt;

    sf::RectangleShape black_square(sf::Vector2f(16*renderer.getScale().x, 16*renderer.getScale().y));
    black_square.setFillColor(sf::Color(0, 0, 0));

    while (fenetre.isOpen())
    {
        dt = c.restart().asMicroseconds()*0.000001f;

        sf::Event event;
        while (fenetre.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                fenetre.close();
        }

        robotloader.updateScripts(dt, mymap);
        sf::Vector2f pos = robotloader.getRobot(0).getPos();
        black_square.setPosition(pos.x*renderer.getScale().x, pos.y*renderer.getScale().y);

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
