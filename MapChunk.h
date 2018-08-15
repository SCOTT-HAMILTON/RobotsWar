#ifndef MAPCHUNK_H
#define MAPCHUNK_H

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>

#include "Renderer.h"
#include "Tileset.h"

using idtype = unsigned long long int;

enum ChunkType{BLOCKSCHUNK, CHUNKSCHUNK};

enum CollideZone{LEFT_SIDE, RIGHT_SIDE, TOP_SIDE, DOWN_SIDE};

class MapChunk
{
public:
    MapChunk(const sf::Vector2f &pos, int nb_tiles_width, int nb_tiles_height, unsigned int divider = 0);
    virtual ~MapChunk();
    virtual void divide(int childdivider = 0);
    bool setBlock(int x, int y, int id);
    void dropToRenderer(Renderer &renderer, const Tileset &tileset, int tile_size);
    void getFreeBlocks(std::vector<sf::Vector2f> &poses, int tile_size) const;
    bool collide(const sf::Vector2f &pos, int tile_size);
    float sweptCollide(const sf::FloatRect &collider, int tile_size, const sf::Vector2f &vel) const;

private:
    sf::FloatRect rect;
    int divider;
    int nb_w;
    int nb_h;
    ChunkType type;
    std::vector<std::vector<MapChunk>> childchunks;
    std::vector<std::vector<int>> blocks;

    static float sweptAABB1(const sf::FloatRect &collider, const sf::FloatRect &block, const sf::Vector2f &velcollider, CollideZone *zone = nullptr);
    static float sweptAABB2(const sf::FloatRect &collider, const sf::FloatRect &block, const sf::Vector2f &velcollider, CollideZone *zone = nullptr);
};

#endif // MAPCHUNK_H
