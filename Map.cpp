#include "Map.h"

std::random_device rd;
std::mt19937 rng(rd());

Map::Map(int nb_tiles_width, int nb_tiles_height) :
    tileset("res/tileset.png", TILE_SIZE)
{
    nb_w = nb_tiles_width;
    nb_h = nb_tiles_height;
    for (std::size_t i = 0; i < nb_h; i++){
        mymap.push_back(std::vector<int>());
        for (std::size_t j = 0; j < nb_w; j++){
            mymap[mymap.size()-1].push_back(0);
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

sf::Vector2f Map::getRandomFreeCase(){
    std::vector<sf::Vector2f> freecases;
    sf::Vector2f pos;
    for (std::size_t i = 0; i < nb_h; i++){
        for (std::size_t j = 0; j < nb_w; j++){
            if (mymap[j][i] == 0){
                freecases.push_back(sf::Vector2f(static_cast<int>(TILE_SIZE*j), static_cast<int>(TILE_SIZE*i)));

            }
        }
    }
    if (freecases.size()>0){
        std::uniform_int_distribution<int> random_freecase_dist(0, freecases.size()-1);
        int index = random_freecase_dist(rng);
        pos = {freecases[index].x, freecases[index].y};
    }
    return pos;
}
