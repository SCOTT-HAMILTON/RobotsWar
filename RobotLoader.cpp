#include "RobotLoader.h"
#include <cstdlib>
#include <cmath>

RobotLoader::~RobotLoader()
{
    //dtor
}
std::vector<std::string> RobotLoader::get_directories(const std::string& s)
{
    std::vector<std::string> r;
    try{
        for(auto& p : fs::recursive_directory_iterator(s)){
            if(p.status().type() == fs::file_type::directory){
                r.push_back(p.path().string());
            }
        }
    }catch(fs::filesystem_error &e){
        std::cout << "error reading robots dir : " << e.what() << '\n';
        std::cout << "app directory : " << fs::current_path() << '\n';
        throw e;
    }

    return r;
}

void RobotLoader::dropRobotsToRenderer(Renderer &renderer){
    for (std::size_t i = 0; i < robots.size(); i++){
        robots[i].dropToRenderer(renderer);
    }
}

void RobotLoader::updateRobotsImg(){
    for (std::size_t i = 0; i < robots.size(); i++){
        robots[i].updateFrame();
    }
}

void RobotLoader::updateScripts(float dt, Map &arenamap){
    std::vector<std::weak_ptr<ScriptCommand>> commands;
    for (std::size_t i = 0; i < robots.size(); i++){
        commands.clear();
        robots[i].initScriptVars(arenamap, dt);
        robots[i].getScriptCommands(6, commands);
        robots[i].updateMissiles(dt, arenamap);
        for (std::size_t c = 0; c < commands.size(); c++){
            std::shared_ptr<ScriptCommand> command = commands[c].lock();
            if (command == nullptr){
                std::cout << "command is nullptr !!\n";
                continue;
            }
            std::string type = command->getType();
            std::cout << "command type : " << type << "\n";
            if (type == "move"){
                command->update();
                sf::Vector2f pos = robots[i].getPos();

                sf::Vector2f off;
                off.x = command->getProp("offsetx");
                off.y = command->getProp("offsety");

                sf::FloatRect rectrobot(pos.x, pos.y, TILE_SIZE, TILE_SIZE);
                sf::Vector2f newpos = arenamap.collide(rectrobot, off);

                robots[i].setPos(newpos);

                robots[i].setScriptVar("posx", robots[i].getPos().x);robots[i].setScriptVar("posy", robots[i].getPos().y);
            }else if (type == "shootmissile"){
                command->update();
                robots[i].shootMissile(command->getProp("offsetx"));
            }else if (type == "shootguidedmissile"){
                idtype id = 0;
                id = robots[i].shootGuidedMissile();
                auto block = command->getBlock().lock();
                if (block != nullptr){
                    block->addVar("return", id);
                }else{
                    std::cout << "nullptr block shootguidedmissile RobotLoader\n";
                }
            }else if (type == "gmissileturn"){
                command->update();
                idtype id = static_cast<idtype>(command->getProp("id"));
                int angle = static_cast<int>(command->getProp("angle"));
                robots[i].setGMissileAngle(id, angle);
            }else if (type == "getgmissilex"){
                command->update();
                idtype id = static_cast<idtype>(command->getProp("id"));
                double x = 0;
                x = robots[i].getGuidedMissileX(id);
                auto block = command->getBlock().lock();
                if (block != nullptr){
                    block->addVar("return", x);
                }else{
                    std::cout << "nullptr block getgmissilex RobotLoader\n";
                }
            }else if (type == "getgmissiley"){
                command->update();
                idtype id = static_cast<idtype>(command->getProp("id"));
                double y = 0;
                y = robots[i].getGuidedMissileY(id);
                auto block = command->getBlock().lock();
                if (block != nullptr){
                    block->addVar("return", y);
                }else{
                    std::cout << "nullptr block getgmissiley RobotLoader\n";
                }
            }else if (type == "destroyblock"){
                command->update();
                int block = static_cast<idtype>(command->getProp("block"));
                if (block > 4 || block < 0)std::cout << "error block " << block << " is invalid !!\n";
                int x = static_cast<int>(robots[i].getPos().x+TILE_SIZE/2);
                int y = static_cast<int>(robots[i].getPos().y+TILE_SIZE/2);
                x = (x-(x%TILE_SIZE)) / TILE_SIZE;
                y = (y-(y%TILE_SIZE)) / TILE_SIZE;
                if (block == BLOCKRIGHT){
                    x++;
                }
                else if (block == BLOCKLEFT){
                    x--;
                }
                else if (block == BLOCKUP){
                    y--;
                }
                else if (block == BLOCKDOWN){
                    y++;
                }
                arenamap.setTile(x, y, 0);
                robots[i].initScriptVars(arenamap, dt);
            }else if (type == "createblock"){
                command->update();
                int blockrel = static_cast<idtype>(command->getProp("blockrel"));
                if (blockrel > 4 || blockrel < 0)std::cout << "error block " << blockrel << " is invalid !!\n";
                int x = static_cast<int>(robots[i].getPos().x+TILE_SIZE/2);
                int y = static_cast<int>(robots[i].getPos().y+TILE_SIZE/2);
                x = (x-(x%TILE_SIZE)) / TILE_SIZE;
                y = (y-(y%TILE_SIZE)) / TILE_SIZE;
                if (blockrel == BLOCKRIGHT){
                    x++;
                }
                else if (blockrel == BLOCKLEFT){
                    x--;
                }
                else if (blockrel == BLOCKUP){
                    y--;
                }
                else if (blockrel == BLOCKDOWN){
                    y++;
                }

                int blockid = command->getProp("blockid");

                arenamap.setTile(x, y, blockid);
                robots[i].initScriptVars(arenamap, dt);
            }else if (type == "print"){
                command->update();
                std::cout << robots[i].getName() << " >> " << command->getStringProp("str") << '\n';
            }else if (type == "varset"){
                command->update();
                //robots[i].setScriptVar(command->getStringProp("varname"), command->getProp("val"));
            }else if (type.rfind("blockentry", 0) == 0){
                command->update();
                if (!command->getProp("canenter")){
                    c += command->getProp("nbcmd");
                }
            }
        }//commands loop
    }//robots loop
}

const Robot &RobotLoader::getRobot(std::size_t index){
    if (index<robots.size())return robots[index];
    else {
        std::cout << "robot at index " << index << " doesn't exist !!\n";
        return robots.back();
    }
}
