#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

#include "Tileset.h"
#include "Renderer.h"

constexpr int TILE_SIZE = 16;

class Map
{
public:
    Map(int nb_tiles_width, int nb_tiles_height);
    virtual ~Map();
    void draw(sf::RenderTarget &target);
    void dropToRenderer(Renderer &renderer);
    sf::Vector2f getRandomFreeCase();

private:
    int nb_w;
    int nb_h;
    Tileset tileset;
    std::vector<std::vector<int>> mymap;
};

#endif // MAP_H
