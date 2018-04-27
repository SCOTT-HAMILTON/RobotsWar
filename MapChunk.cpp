#include "MapChunk.h"

idtype MapChunk::cur_id = 0;

MapChunk::MapChunk(const sf::Vector2f &pos, int nb_tiles_width, int nb_tiles_height, unsigned int divider) :
    nb_w(nb_tiles_width), nb_h(nb_tiles_height), myid(cur_id)
{
    rect = sf::FloatRect(pos.x, pos.y, nb_w, nb_h);
    if (myid == 23)std::cout << "new Chunk : " << rect.left << ", " << rect.top << ", " << rect.width << ", " << rect.height << std::endl;
    this->divider = divider;
    if (divider > 1 && nb_tiles_height%divider+nb_tiles_width%divider == 0){
        type = CHUNKSCHUNK;
    }else{
        type = BLOCKSCHUNK;

        for (std::size_t i = 0; i < nb_tiles_height; i++){
            blocks.push_back(std::vector<int>());
            for (std::size_t j = 0; j < nb_tiles_width; j++){
                blocks.back().push_back(0);
            }
        }
        if (myid == 23) std::cout << "type 23 is BLOCKSCHUNK" << std::endl;
    }

    cur_id++;
}

MapChunk::~MapChunk()
{
    //dtor
}

bool MapChunk::setBlock(int x, int y, int id){
    if (rect.contains(x, y)){
        if (type == BLOCKSCHUNK){
            blocks[y-rect.top][x-rect.left] = id;
            return true;
        }else{
            int tmp_x = (x-rect.left)/divider;
            int tmp_y = (y-rect.top)/divider;
            return childchunks[tmp_y][tmp_x].setBlock(x, y, id);
        }
    }
    return false;
}

void MapChunk::divide(int childdivider){
    int chunk_width = nb_w/divider;
    int chunk_height = nb_h/divider;
    for (std::size_t i = 0; i < chunk_height; i++){
        childchunks.push_back(std::vector<MapChunk>());
        for (std::size_t j = 0; j < chunk_width; j++){
            sf::Vector2f pos(rect.left+j*chunk_width, rect.top+i*chunk_height);
            //std::cout << "new chunk : " << pos.x << ", " << pos.y << std::endl;
            childchunks.back().push_back(MapChunk(pos, chunk_width, chunk_height, childdivider));
        }

    }
}

void MapChunk::dropToRenderer(Renderer &renderer, const Tileset &tileset, int tile_size){
    if (type == BLOCKSCHUNK){
        for (std::size_t i = 0; i < blocks.size(); i++){
            for (std::size_t j = 0; j < blocks[i].size(); j++){
                sf::IntRect textrect = tileset.getRectFromId(blocks[i][j]);
                sf::Vector2f pos(sf::Vector2f(j*tile_size+rect.left*tile_size, i*tile_size+rect.top*tile_size));
                renderer.addRenderItem((sf::Texture*)tileset.getTexture(), pos, textrect);
            }
        }

    }else{
        for (std::size_t i = 0; i < childchunks.size(); i++){
            for (std::size_t j = 0; j < childchunks[i].size(); j++){
                childchunks[i][j].dropToRenderer(renderer, tileset, tile_size);
            }
        }
    }
}

void MapChunk::getFreeBlocks(std::vector<sf::Vector2f> &poses, int tile_size) const{
    if (type == BLOCKSCHUNK){
        for (std::size_t i = 0; i < blocks.size(); i++){
            for (std::size_t j = 0; j < blocks[i].size(); j++){
                if (blocks[i][j] == 0)poses.emplace_back(j*tile_size+rect.left*tile_size, i*tile_size+rect.top*tile_size);
            }
        }
    }else{
        for (std::size_t i = 0; i < childchunks.size(); i++){
            for (std::size_t j = 0; j < childchunks[i].size(); j++){
                childchunks[i][j].getFreeBlocks(poses, tile_size);
            }
        }
    }
}

bool MapChunk::collide(const sf::Vector2f &pos, int tile_size){
    if (!rect.contains(pos))return false;
    if (type == BLOCKSCHUNK){
        for (std::size_t i = 0; i < blocks.size(); i++){
            for (std::size_t j = 0; j < blocks[i].size(); j++){
                sf::FloatRect tmprect(j*tile_size+rect.left*tile_size, i*tile_size+rect.top*tile_size, tile_size, tile_size);
                if (blocks[i][j] != 0 && tmprect.contains(pos))return true;
            }
        }
    }else{
        for (std::size_t i = 0; i < childchunks.size(); i++){
            for (std::size_t j = 0; j < childchunks[i].size(); j++){
                if (childchunks[i][j].collide(pos, tile_size))return true;
            }
        }
    }

    return false;
}

bool MapChunk::collide(const sf::FloatRect &_rect, int tile_size){
    sf::FloatRect realrect(rect.left*tile_size, rect.top*tile_size, rect.width*tile_size, rect.height*tile_size);
    if (!realrect.intersects(_rect)){
        return false;
    }
    if (type == BLOCKSCHUNK){
        for (std::size_t i = 0; i < blocks.size(); i++){
            for (std::size_t j = 0; j < blocks[i].size(); j++){
                sf::FloatRect tmprect(j*tile_size+rect.left*tile_size, i*tile_size+rect.top*tile_size, tile_size, tile_size);
                if (blocks[i][j] != 0 && tmprect.intersects(_rect))return true;
            }
        }
    }else{
        for (std::size_t i = 0; i < childchunks.size(); i++){
            for (std::size_t j = 0; j < childchunks[i].size(); j++){
                if (childchunks[i][j].collide(_rect, tile_size))return true;
            }
        }
    }

    return false;
}
