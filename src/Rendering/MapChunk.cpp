#include "Rendering/MapChunk.h"


MapChunk::MapChunk(const sf::Vector2f &pos, int nb_tiles_width, int nb_tiles_height, int _divider) :
    divider(_divider), nb_w(nb_tiles_width), nb_h(nb_tiles_height)
{
    rect = sf::FloatRect(pos.x, pos.y, nb_w, nb_h);
    if (divider > 1 && nb_tiles_height%divider+nb_tiles_width%divider == 0){
        type = CHUNKSCHUNK;
    }else{
        type = BLOCKSCHUNK;

        for (int i = 0; i < nb_tiles_height; i++){
            blocks.push_back(std::vector<int>());
            for (int j = 0; j < nb_tiles_width; j++){
                blocks.back().push_back(0);
            }
        }
    }
}

bool MapChunk::setBlock(int x, int y, int id){
    if (rect.contains(x, y)){
        if (type == BLOCKSCHUNK){
            blocks[static_cast<size_t>(y-rect.top)][static_cast<size_t>(x-rect.left)] = id;
            return true;
        }else{
            int tmp_x = static_cast<int>(x-rect.left)/divider;
            int tmp_y = static_cast<int>(y-rect.top)/divider;
            return childchunks[static_cast<size_t>(tmp_y)][static_cast<size_t>(tmp_x)].setBlock(x, y, id);
        }
    }
    return false;
}

void MapChunk::divide(int childdivider){
    int chunk_width = nb_w/divider;
    int chunk_height = nb_h/divider;
    for (int i = 0; i < chunk_height; i++){
        childchunks.push_back(std::vector<MapChunk>());
        for (int j = 0; j < chunk_width; j++){
            sf::Vector2f pos(rect.left+j*chunk_width, rect.top+i*chunk_height);
            childchunks.back().push_back(MapChunk(pos, chunk_width, chunk_height, childdivider));
        }

    }
}

void MapChunk::dropToRenderer(Renderer &renderer, const Tileset &tileset, int tile_size){
    if (type == BLOCKSCHUNK){
        for (std::size_t i = 0; i < blocks.size(); i++){
            for (std::size_t j = 0; j < blocks[i].size(); j++){
                sf::IntRect textrect = tileset.getRectFromId(blocks[i][j]);
                sf::Vector2f pos(sf::Vector2f(static_cast<int>(j)*tile_size+rect.left*tile_size, static_cast<int>(i)*tile_size+rect.top*tile_size));
                renderer.addRenderItem(tileset.getTexture(), pos, textrect);
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
                if (blocks[i][j] == 0)poses.emplace_back(j*static_cast<size_t>(tile_size)+rect.left*tile_size, i*static_cast<size_t>(tile_size)+rect.top*tile_size);
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
    size_t startx = static_cast<size_t>((pos.x - rect.left*tile_size)/tile_size);
    size_t starty = static_cast<size_t>((pos.y - rect.top*tile_size)/tile_size);
    if (type == BLOCKSCHUNK){
        sf::FloatRect tmprect(startx*static_cast<unsigned long>(tile_size)+rect.left*tile_size, starty*static_cast<unsigned long>(tile_size)+rect.top*tile_size, tile_size, tile_size);
        if (blocks[starty][startx] != 0 && tmprect.contains(pos))return true;
    }else{
        for (std::size_t i = 0; i < childchunks.size(); i++){
            for (std::size_t j = 0; j < childchunks[i].size(); j++){
                if (childchunks[i][j].collide(pos, tile_size))return true;
            }
        }
    }

    return false;
}

float MapChunk::sweptCollide(const sf::FloatRect &collider, int tile_size, const sf::Vector2f &velcollider) const{
    //std::cerr << "\nswept collide !!!\n"';
    sf::FloatRect rectrange;
    int marge = 1;

    sf::FloatRect tmpcolliderrect(collider);
    tmpcolliderrect.left += marge;
    tmpcolliderrect.top += marge;
    tmpcolliderrect.width -= marge*2;
    tmpcolliderrect.height -= marge*2;

    float maxx, maxy, minx, miny;
    maxx = collider.left+collider.width;
    minx = collider.left;
    if (velcollider.x>0){
        maxx += velcollider.x;
    }else minx += velcollider.x;
    maxy = collider.top+collider.height;
    miny = collider.top;
    if (velcollider.y>0){
        maxy += velcollider.y;
    }else miny += velcollider.y;
    //std::cerr << "minx, maxx, miny, maxy : " << minx << ", " << maxx << ", " << miny << ", " << maxy << '\n';
    rectrange.left = minx;
    rectrange.top = miny;
    rectrange.width = maxx-minx;
    rectrange.height = maxy-miny;

    sf::FloatRect realrect(rect.left*tile_size, rect.top*tile_size, rect.width*tile_size, rect.height*tile_size);

    if (!realrect.intersects(rectrange)){
        //std::cerr << "my rect out : " << realrect.left << ", " << realrect.top << ", " << realrect.width << ", " << realrect.height << '\n';
        //std::cerr << "rectrange : " << rectrange.left << ", " << rectrange.top << ", " << rectrange.width << ", " << rectrange.height << '\n';
        return 1.0f;

    }


    size_t startx = static_cast<size_t>((rectrange.left - realrect.left)/tile_size);
    size_t starty = static_cast<size_t>((rectrange.top - realrect.top)/tile_size);
    startx = (startx>0) ? startx-1 : startx;
    starty = (starty>0) ? starty-1 : starty;

    size_t endx = static_cast<size_t>((rectrange.left+rectrange.width - realrect.left)/tile_size);
    size_t endy = static_cast<size_t>((rectrange.top+rectrange.height - realrect.top)/tile_size);
    endx++;
    endy++;

    float min_swept = 1;

    if (type == BLOCKSCHUNK){
        //std::cerr << "blocks chunk !!\n"';
        for (std::size_t i = starty; i < blocks.size() && i < endy; i++){
            for (std::size_t j = startx; j < blocks[i].size() && j < endx; j++){
                if (blocks[i][j] != 0){
                    sf::FloatRect tmprect(static_cast<int>(j)*tile_size+realrect.left, static_cast<int>(i)*tile_size+realrect.top, tile_size, tile_size);
                    sf::FloatRect newcolliderrect(tmpcolliderrect);
                    newcolliderrect.left = tmpcolliderrect.left+velcollider.x;
                    newcolliderrect.top = tmpcolliderrect.top+velcollider.y;
                    if (tmprect.intersects(newcolliderrect)){
                        return 0.0f;
                    }
                    float swept = sweptAABB2(tmpcolliderrect, tmprect, velcollider);
                    if (swept<min_swept)min_swept = swept;
                }
            }
        }
    }else{
        for (std::size_t i = 0; i < childchunks.size(); i++){
            for (std::size_t j = 0; j < childchunks[i].size(); j++){
                float swept = childchunks[i][j].sweptCollide(collider, tile_size, velcollider);
                if (swept<min_swept)min_swept = swept;
            }
        }
    }

    //std::cerr << "min swept : " << min_swept << '\n';

    return min_swept;
}

float MapChunk::sweptAABB2(const sf::FloatRect &b1, const sf::FloatRect &b2, const sf::Vector2f &vel, CollideZone * /*zone*/){
    sf::FloatRect newRect(b1.left+vel.x, b1.top+vel.y, b1.width, b1.height);
    if (newRect.intersects(b2)){
        return 0.00f;
    }

    int abs_diffx = static_cast<int>(std::abs(b2.left - b1.left));
    int abs_diffy = static_cast<int>(std::abs(b2.top - b1.top));
    float swept = 0;
    if (abs_diffx>abs_diffy ){//X axis LEFT RIGHT
        if (b1.left<b2.left){//LEFT
            float abs_leftright = std::abs(b2.left-(b1.left+b1.width));
            swept = abs_leftright/vel.x;
        }else{//RIGHT
            float abs_rightleft = std::abs((b2.left+b2.width)-b1.left);
            swept = abs_rightleft/vel.x;
        }
    }else{ // Y axis TOP DOWN
        if (b1.top<b2.top){//TOP
            float abs_topdown = std::abs(b2.top-(b1.top+b1.height));
            swept = abs_topdown/vel.y;
        }else{//DOWN
            float abs_downtop = std::abs((b2.top+b2.height)-b1.top);
            swept = abs_downtop/vel.y;
        }
    }

    sf::FloatRect b2withmarge(b2);
    //b2withmarge.width++;
    //b2withmarge.height++;

    sf::Vector2f collisionpt = {b1.left+vel.x*swept, b1.top+vel.y*swept};
    if (!b2withmarge.contains(collisionpt)){
        swept = 1;
    }

    swept = (swept > 1) ? 1 : ( (swept < 0) ? 0 : swept ) ;

    return swept;
}

float MapChunk::sweptAABB1(const sf::FloatRect &b1, const sf::FloatRect &b2, const sf::Vector2f &vel, CollideZone *zone){
    float dist_left_left, dist_left_right;
    float dist_top_top, dist_top_down;

    sf::Vector2f collisionpt;

    sf::FloatRect b2rectmarge(b2);
    b2rectmarge.width++;
    b2rectmarge.height++;

    dist_left_left = std::abs(b2.left-b1.left);
    dist_left_right = std::abs(b1.left-(b2.left+b2.width));

    dist_top_top = std::abs(b2.top-b1.top);
    dist_top_down = std::abs(b1.top-(b2.top+b2.height));

    float abs_velx = std::abs(vel.x);
    float abs_vely = std::abs(vel.y);

    float swept = 1;

    float result = 1;
    sf::Vector2f pos(0, 0);

    swept = dist_top_down/abs_vely;

    if (swept<0)swept = 0;
    else if (swept>1)swept = 1;

    collisionpt = {b1.left+vel.x*swept, b1.top+vel.y*swept};
    if (b2rectmarge.contains(collisionpt)){
        if (swept<result){
            std::cerr << "top down  !!\n";
            if (zone != nullptr)*zone = DOWN_SIDE;
            result = swept;
            pos = collisionpt;
        }
    }

    swept = dist_left_left/abs_velx;

    if (swept<0)swept = 0;
    else if (swept>1)swept = 1;

    collisionpt = {b1.left+vel.x*swept, b1.top+vel.y*swept};
    if (b2rectmarge.contains(collisionpt)){
        if (swept<result){
            std::cerr << "left left  !!\n";
            if (zone != nullptr)*zone = LEFT_SIDE;
            result = swept;
            pos = collisionpt;
        }
    }

    swept = dist_left_right/abs_velx;

    if (swept<0)swept = 0;
    else if (swept>1)swept = 1;

    collisionpt = {b1.left+vel.x*swept, b1.top+vel.y*swept};
    if (b2rectmarge.contains(collisionpt)){
        if (swept<result){
            result = swept;
            pos = collisionpt;
        }
    }

    swept = dist_top_top/abs_vely;

    if (swept<0)swept = 0;
    else if (swept>1)swept = 1;

    collisionpt = {b1.left+vel.x*swept, b1.top+vel.y*swept};
    if (b2rectmarge.contains(collisionpt)){
        if (swept<result){
            std::cerr << "top top  !!\n";
            if (zone != nullptr)*zone = TOP_SIDE;
            result = swept;
            pos = collisionpt;
        }
    }

    if (result<1){
        collisionpt = pos;

        return result;
    }

    collisionpt = {b1.left+vel.x, b1.top+vel.y};

    return 1.0f;
}
