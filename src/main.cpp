#include <iostream>
#include <SFML/Graphics.hpp>

#include "Rendering/Map.h"
#include "Rendering/Renderer.h"
#include "RobotLoader.h"

int main()
{
    Robot::initTexts();

    Map mymap(18, 18);
    Renderer renderer;
    renderer.setScale(sf::Vector2f(2, 2));

    RobotLoader robotloader;

    try{
        robotloader.loadRobots(mymap);
    }catch(fs::filesystem_error & /*e*/){
        std::cerr << "exiting...\n";
        return 0;
    }
    sf::RenderWindow fenetre(sf::VideoMode(static_cast<unsigned int>(mymap.getNbTilesWidth()*TILE_SIZE*renderer.getScale().x), static_cast<unsigned int>(mymap.getNbTilesHeight()*TILE_SIZE*renderer.getScale().y)), "RobotsWar !!!");
    fenetre.setFramerateLimit(200);
    fenetre.setVerticalSyncEnabled(true);

    sf::Font font;font.loadFromFile("res/absender1.ttf");
    sf::Text frameratestext("0 FPS", font, 20);
    frameratestext.setPosition(10, 10);
    frameratestext.setFillColor(sf::Color::Black);

    sf::Clock c;
    sf::Clock fpsupdate;
    float dt = 0.0;
    sf::Clock timer;
    bool pause = false;

    unsigned long framecounter = 0;


    sf::RectangleShape black_square(sf::Vector2f(16*renderer.getScale().x, 16*renderer.getScale().y));
    black_square.setFillColor(sf::Color(0, 0, 0));

    while (fenetre.isOpen())
    {
        framecounter++;
        float clockt = c.restart().asMicroseconds();
        float lastdt = dt;
        dt = static_cast<float>(clockt*0.000001f);// equal to 1 for a 1 fps framerate
        if (dt > 0.1f){
            dt = lastdt;
        }
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
        sf::Vector2f pos = robotloader.getRobotPos(0);
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
