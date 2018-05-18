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
    sf::Vector2f collide(const sf::FloatRect &rect, const sf::Vector2f &vel);
    int getNbTilesWidth() const;
    int getNbTilesHeight() const;
    int getTile(int x, int y) const;

private:
    int nb_w;
    int nb_h;
    Tileset tileset;
    std::vector<std::vector<MapChunk>> chunks;
    std::vector<std::vector<int>> idmap;
    int chunk_width;
    int chunk_height;

};

#endif // MAP_H
