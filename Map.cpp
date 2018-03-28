#include "Map.h"

Map::Map(int nb_tiles_width, int nb_tiles_height) :
    tileset("res/tileset.png", TILE_SIZE)
{
    nb_w = nb_tiles_width;
    nb_h = nb_tiles_height;
    for (std::size_t i = 0; i < nb_h; i++){
        mymap.push_back(std::vector<int>());
        for (std::size_t j = 0; j < nb_w; j++){
            mymap[mymap.size()-1].push_back(9);
        }
    }

}

Map::~Map()
{
    //dtor
}

void Map::dropToRenderer(Renderer &renderer){
    for (std::size_t i = 0; i < nb_h; i++){
        for (std::size_t j = 0; j < nb_w; j++){
            sf::IntRect rect = tileset.getRectFromId(mymap[j][i]);
            renderer.addRenderItem((sf::Texture*)tileset.getTexture(), sf::Vector2f(j*TILE_SIZE, i*TILE_SIZE), tileset.getRectFromId(mymap[j][i]));
        }
    }
}
