#include "Robot.h"

Robot::Robot(const std::string &author, int nb_frames, const std::string &dir) :
    path(dir), reader(path+"/script.txt")
{
    std::cout << "lol " << std::endl;
    std::size_t index = path.find_last_of("/\\")+1;
    if (index == path.npos)name = "defaultname";
    else name = path.substr(index , path.size());
    std::cout << "name : " << name << std::endl;
    this->author = author;
    this->nb_frames = nb_frames;
    sf::Image img;
    img.loadFromFile(dir+"/img.png");
    texture.loadFromImage(img);
    id_img = 0;
    tile_size = img.getSize().x/nb_frames;
    updateRect();
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

const sf::Vector2f &Robot::getPos() const
{
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

void Robot::getScriptCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands){
    reader.getCommands(nbCommands, commands);
}

int Robot::scriptNbCommands(){
    return reader.nbCommands();
}

double Robot::getScriptVar(const std::string &var){
    return reader.getVar(var);
}

void Robot::setScriptVar(const std::string &var, float val){
    reader.setVar(var, val);
}

void Robot::displayScriptVars(){
    reader.displayMainBlockVars();
}

const std::string &Robot::getName(){
    return name;
}
