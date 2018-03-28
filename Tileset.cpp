#include "Tileset.h"

Tileset::Tileset(const std::string &tilespath, const int tile_size)
{
    this->tile_size = tile_size;
    img.loadFromFile(tilespath);
    tileset.loadFromImage(img);
    nb_w = static_cast<int>(img.getSize().x/tile_size);
    nb_h = static_cast<int>(img.getSize().y/tile_size);
}

Tileset::~Tileset()

{
    //dtor
}

sf::IntRect Tileset::getRectFromId(int id){
    sf::IntRect result(0, 0, tile_size, tile_size);
    result.left = (id%nb_w)*tile_size;
    result.top = (id-(id%nb_w))/nb_w*tile_size;
    return result;
}

const sf::Texture *Tileset::getTexture(){
    return &tileset;
}
