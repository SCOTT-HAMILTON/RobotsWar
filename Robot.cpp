#include "Robot.h"

std::map<int, sf::Texture> Robot::missiletexts = missiletexts;
std::map<int, sf::Texture> Robot::guidedmissiletexts = guidedmissiletexts;
sf::IntRect Robot::missilesize = sf::IntRect(0, 0, 14, 6);
sf::IntRect Robot::guidedmissilesize = sf::IntRect(0, 0, 8, 8);
float Robot::missilespeed = 100;
int Robot::msTimeDeathMissile = 1000;

void Robot::initTexts(){
}

Robot::Robot(const std::string &author, int nb_frames, const std::string &dir, const sf::Vector2f &pos, const Map &worldmap) :
    path(dir), curGuidedMissileId(0)
{
    this->pos = pos;

    reader.initVars(pos, worldmap, 0);
    reader.load(path+"/script.txt");

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
    for (size_t i = 0; i < missiles.size(); i++){
        sf::Vector2f pos = missiles[i].pos;
        int a = 255;
        if (missiles[i].dying){
            int t = missiles[i].cooldown_death.getElapsedTime().asMilliseconds();
            if (t > msTimeDeathMissile)t = msTimeDeathMissile;
            t = msTimeDeathMissile-t;
            a = t*255/msTimeDeathMissile;
        }
        if (missiletexts.find(a) == missiletexts.end()){
            missiletexts.emplace(a, sf::Texture());
            sf::Image img;
            img.create(missilesize.width, missilesize.height, sf::Color(0, 0, 255, a));
            missiletexts[a].loadFromImage(img);
        }
        renderer.addRenderItem(&missiletexts[a], pos, missilesize);
    }

    for (auto missile = guidedmissiles.begin(); missile != guidedmissiles.end();missile++){
        sf::Vector2f pos = missile->second.pos;
        int a = 255;
        if (missile->second.dying){
            int t = missile->second.cooldown_death.getElapsedTime().asMilliseconds();
            if (t > msTimeDeathMissile)t = msTimeDeathMissile;
            t = msTimeDeathMissile-t;
            a = t*255/msTimeDeathMissile;
        }
        if (guidedmissiletexts.find(a) == guidedmissiletexts.end()){
            guidedmissiletexts.emplace(a, sf::Texture());
            sf::Image img;
            img.create(guidedmissilesize.width, guidedmissilesize.height, sf::Color(0, 0, 255, a));
            guidedmissiletexts[a].loadFromImage(img);
        }
        renderer.addRenderItem(&guidedmissiletexts[a], pos, guidedmissilesize);
    }
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

void Robot::initScriptVars(const Map &worldmap, float dt){
    reader.initVars(pos, worldmap, dt);
}

void Robot::shootMissile(int direction){
    if (direction == 1){
        missiles.push_back( {{pos.x, pos.y+5}, 1, false} );
    }else{
        missiles.push_back( {{pos.x, pos.y+5}, -1, false} );
    }
}

idtype Robot::shootGuidedMissile(){
    Missile m = {{pos.x, pos.y+5}, 0, false};
    guidedmissiles.emplace(curGuidedMissileId, m );
    curGuidedMissileId++;
    return curGuidedMissileId-1;
}

void Robot::updateMissiles(float dt, const Map &arenamap){
    for (int i = missiles.size()-1; i >= 0; i--){
        if (missiles[i].dying && missiles[i].cooldown_death.getElapsedTime().asMilliseconds() >= msTimeDeathMissile){
            missiles.erase(missiles.begin()+i);
            continue;
        }
        sf::Vector2f off(dt*missilespeed*missiles[i].direction, 0);
        sf::Vector2f pos = missiles[i].pos;
        missiles[i].pos = arenamap.collide(sf::FloatRect(pos.x, pos.y, missilesize.width, missilesize.height), off);
        if (!missiles[i].dying && (missiles[i].pos == pos || missiles[i].cooldown_death.getElapsedTime().asMilliseconds() > 1000)){
            missiles[i].cooldown_death.restart();
            missiles[i].dying = true;
        }
    }

    for (auto it = guidedmissiles.end(); guidedmissiles.size()>0; it--){
        if (it!=guidedmissiles.end()){
            if (it->second.dying && it->second.cooldown_death.getElapsedTime().asMilliseconds() >= msTimeDeathMissile){
                guidedmissiles.erase(it);
                continue;
            }

            sf::Vector2f pos = it->second.pos;
            float angle = it->second.direction*M_PI/180;
            sf::Vector2f off;
            off.x = std::cos(angle)*dt*missilespeed;
            off.y = std::sin(angle)*dt*missilespeed;
            it->second.pos = arenamap.collide(sf::FloatRect(pos.x, pos.y, guidedmissilesize.width, guidedmissilesize.height), off);

            if (!it->second.dying && it->second.cooldown_death.getElapsedTime().asMilliseconds() > 10000){
                it->second.cooldown_death.restart();
                it->second.dying = true;
            }
            if (it == guidedmissiles.begin())break;
        }
    }
}

void Robot::setGMissileAngle(idtype id, int angle){
    if (guidedmissiles.find(id) != guidedmissiles.end()){
        guidedmissiles[id].direction = angle;
    }else{
        std::cout << "ERROR, id not correct missile n " << id << " doesn't exist !!" << std::endl;
    }
}

double Robot::getGuidedMissileX(idtype id) const{
    if (guidedmissiles.find(id) != guidedmissiles.end()){
        return guidedmissiles.at(id).pos.x;
    }else{
        std::cout << "ERROR, id not correct missile n " << id << " doesn't exist !!" << std::endl;
    }
}

double Robot::getGuidedMissileY(idtype id) const{
    if (guidedmissiles.find(id) != guidedmissiles.end()){
        return guidedmissiles.at(id).pos.y;
    }else{
        std::cout << "ERROR, id not correct missile n " << id << " doesn't exist !!" << std::endl;
    }
}
