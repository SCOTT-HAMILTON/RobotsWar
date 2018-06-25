#include "Map.h"

#include <cmath>

Map::Map(int nb_tiles_width, int nb_tiles_height) :
    tileset("res/tileset.png", TILE_SIZE)
{
    std::srand(std::time(NULL));
    nb_w = nb_tiles_width;
    nb_h = nb_tiles_height;
    std::cout << "TILE SIZE : " << TILE_SIZE << std::endl;


    chunk_width = static_cast<int>( (nb_tiles_width-nb_tiles_width%2) /2);
    chunk_height = static_cast<int>((nb_tiles_height-nb_tiles_height%2)/2);
    for (std::size_t i = 0; i < 2; i++){
        chunks.push_back(std::vector<MapChunk>());
        for (std::size_t j = 0; j < 2; j++){
            sf::Vector2f pos(j*chunk_width, i*chunk_height);
            chunks.back().push_back(MapChunk(pos, chunk_width, chunk_height, 3));
            chunks.back().back().divide();
        }

    }
    for (std::size_t i = 0; i < nb_h; i++){
        idmap.push_back(std::vector<int>());
        for (std::size_t j = 0; j < nb_w; j++){
            int id = rand()%1000;
            if (id>400)id = 1;
            else id = 0;
            int tmp_x = j/chunk_width;
            int tmp_y = i/chunk_height;
            chunks[tmp_y][tmp_x].setBlock(j, i, id);
            idmap.back().push_back(id);
        }
    }
}

Map::~Map()
{
    //dtor
}

void Map::dropToRenderer(Renderer &renderer){
    for (std::size_t i = 0; i < chunks.size(); i++){
        for (std::size_t j = 0; j < chunks[i].size(); j++){
            chunks[i][j].dropToRenderer(renderer, tileset, TILE_SIZE);
        }
    }
}

sf::Vector2f Map::getRandomFreeCase(){
    std::vector<sf::Vector2f> freecases;
    sf::Vector2f pos;

    for (std::size_t i = 0; i < chunks.size(); i++){
        for (std::size_t j = 0; j < chunks[i].size(); j++){
            chunks[i][j].getFreeBlocks(freecases, TILE_SIZE);
        }
    }

    if (freecases.size()>0){
        int index = rand()%freecases.size();
        pos = {freecases[index].x, freecases[index].y};
    }

    std::cout << "pos random found : " << pos.x << ", " << pos.y << std::endl;

    return pos;
}

bool Map::collide(const sf::Vector2f &pos){
    int tmp_x = std::round(pos.x)/TILE_SIZE/chunk_width;
    int tmp_y = std::round(pos.y)/TILE_SIZE/chunk_height;
    if (tmp_x<0 || tmp_x>1 || tmp_y<0 || tmp_y>1)return false;
    return chunks[tmp_x][tmp_y].collide(pos, TILE_SIZE);
}

sf::Vector2f Map::collide(const sf::FloatRect &rect, const sf::Vector2f &vel) const{
    float min_swept = 1.0f;
    for (std::size_t i = 0; i < chunks.size(); i++){
        for (std::size_t j = 0; j < chunks[i].size(); j++){
            float swept = chunks[i][j].sweptCollide(rect, TILE_SIZE, vel);
            if (swept<min_swept)min_swept = swept;
        }
    }
    if (min_swept<0)min_swept = 0;
    //if (min_swept<0.3f)min_swept = 0;
    sf::Vector2f pos( rect.left+vel.x*min_swept, rect.top+vel.y*min_swept );
    /*
    if (min_swept == 0){
        float mv_x, mv_y;
        int mod_x = static_cast<int>(pos.x+rect.width)%TILE_SIZE;
        int mod_y = static_cast<int>(pos.y+rect.height)%TILE_SIZE;
        if (mod_x>=TILE_SIZE/2){
            mv_x = TILE_SIZE-mod_x;
        }else{
            mv_x = -mod_x;
        }
        if (mod_y>=TILE_SIZE/2){
            mv_y = TILE_SIZE-mod_y;
        }else{
            mv_y = -mod_y;
        }

        if (mv_y<mv_x){
            pos.y = static_cast<int>(pos.y)+mv_y;
        }else {
            pos.x = static_cast<int>(pos.x)+mv_x;
        }
    }*/
    return pos;
}

int Map::getNbTilesWidth() const{
    return nb_w;
}

int Map::getNbTilesHeight() const{
    return nb_h;

}

int Map::getTile(int x, int y) const{
    if (x<0 || y<0 || x>=nb_w || y>=nb_h)return -1;
    return idmap[y][x];
}

int Map::setTile(int x, int y, int id){
    if (x<0 || y<0 || x>=nb_w || y>=nb_h) return 1;
    idmap[y][x] = id;

    int tmp_x = x/chunk_width;
    int tmp_y = y/chunk_height;

    chunks[tmp_y][tmp_x].setBlock(x, y, id);
    return 0;
}
