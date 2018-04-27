#ifndef TILESET_H
#define TILESET_H

#include <vector>
#include <SFML/Graphics.hpp>
#include <iostream>

class Tileset
{
public:
    Tileset(const std::string &tilespath, const int tile_size);
    virtual ~Tileset();
    sf::IntRect getRectFromId(int id) const;
    const sf::Texture *getTexture() const;

private:
    int nb_w;
    int nb_h;
    sf::Image img;
    int tile_size;
    sf::Texture tileset;
};

#endif // TILESET_H
