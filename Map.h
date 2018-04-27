#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>

#include "Tileset.h"
#include "Renderer.h"
#include "MapChunk.h"

constexpr int TILE_SIZE = 16;

class Map
{
public:
    Map(int nb_tiles_width, int nb_tiles_height);
    virtual ~Map();
    void dropToRenderer(Renderer &renderer);
    sf::Vector2f getRandomFreeCase();
    bool collide(const sf::Vector2f &pos);
    bool collide(const sf::FloatRect &rect);
    int getNbTilesWidth() const;
    int getNbTilesHeight() const;

private:
    int nb_w;
    int nb_h;
    Tileset tileset;
    std::vector<std::vector<MapChunk>> chunks;
    int chunk_width;
    int chunk_height;

};

#endif // MAP_H
