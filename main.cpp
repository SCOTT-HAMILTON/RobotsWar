#include <iostream>
#include <SFML/Graphics.hpp>

#include "Map.h"
#include "Renderer.h"

int main()
{
    Map mymap(40, 40);
    Renderer renderer;

    sf::RenderWindow fenetre(sf::VideoMode(40*TILE_SIZE, 40*TILE_SIZE), "SFML works!");
    fenetre.setFramerateLimit(144);

    while (fenetre.isOpen())
    {
        sf::Event event;
        while (fenetre.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                fenetre.close();
        }

        //RENDERER DROPPING
        mymap.dropToRenderer(renderer);

        //RENDERER RENDER
        fenetre.clear();
        renderer.render(fenetre);
        fenetre.display();
    }

    return 0;
}
