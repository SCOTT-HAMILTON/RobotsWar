#include "Robot.h"

Robot::Robot(const std::string &author, int nb_frames, const std::string &dir) :
    path(dir), reader(path+"/script.txt")
{
    this->author = author;
    this->nb_frames = nb_frames;
    sf::Image img;
    img.loadFromFile(dir+"/img.png");
    texture.loadFromImage(img);
    id_img = 0;
    tile_size = img.getSize().x/nb_frames;
    updateRect();
    std::cout << "rect : " << rect.left << ", " << rect.top << ", " << rect.width << ", " << rect.height << std::endl;

}


Robot::~Robot()
{
    //dtor
}

void Robot::updateRect(){
    rect.left = (id_img%nb_frames)*tile_size;
    rect.top = 0;
    rect.width = tile_size;
    rect.height = texture.getSize().y;
}

void Robot::dropToRenderer(Renderer &renderer){
    renderer.addRenderItem(&texture, pos, rect);
}

void Robot::setPos(const sf::Vector2f &pos){
    this->pos = pos;
}

const sf::Vector2f &Robot::getPos(){
    return pos;
}

void Robot::updateFrame(){
    if (imgClock.getElapsedTime().asMilliseconds()>1000/5){
        imgClock.restart();
        id_img++;
        if (id_img>=nb_frames){
            id_img = 0;
        }
        updateRect();
    }
}

const std::string &Robot::getPath(){
    return path;
}

void Robot::getScriptCommands(std::size_t start, std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands){
    reader.getCommands(start, nbCommands, commands);
}

int Robot::scriptNbCommands(){
    return reader.nbCommands();
}

const void* Robot::getScriptVar(const std::string &var){
    return reader.getVar(var);
}
