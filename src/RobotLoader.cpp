#include "RobotLoader.h"
#include <cstdlib>
#include <cmath>



RobotLoader::~RobotLoader()
{
    //dtor
}

RobotLoader::RobotLoader()
{
}

void RobotLoader::loadRobots(const Map &worldmap)
{
    std::vector<std::string> dirs;
    std::string file_path;
    try{
        dirs = get_directories("robots");
    }catch(fs::filesystem_error &e){
        throw e;
    }

    for (std::size_t i = 0; i < dirs.size(); i++){
        std::ifstream file_opt(dirs[i]+"/prop.txt");
        if (!file_opt.good())continue;
        bool continuer = true;
        char buffer[1000];
        std::string author;
        int nb_frames = 0;
        while (continuer && !file_opt.eof()){
            file_opt.getline(buffer, sizeof(buffer));
            std::string line(buffer);
            if (line.rfind("author:", 0) == 0) {
                author = line.substr(7, line.size()-7);
                std::cerr << "author : " << author << '\n';
            }else if (line.rfind("nb_frames:", 0) == 0) {
                std::string tmpstr = line.substr(10, line.size()-7);
                try {
                    nb_frames = std::stoi(tmpstr);
                }catch (std::exception &e){
                    std::cerr << "error converting string " << tmpstr << " to int : " << e.what() << '\n';;
                }
                std::cerr << "nb_frames : " << nb_frames << '\n';
            }
        }

        if (std::isnan(nb_frames))continue;
        auto tmprobot = std::make_unique<Robot>(author, nb_frames, dirs[i], worldmap.getRandomFreeCase(), worldmap);
        robots.push_back(std::move( tmprobot ));
        std::cerr << '\n';
    }
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
        std::cerr << "error reading robots dir : " << e.what() << '\n';
        std::cerr << "app directory : " << fs::current_path() << '\n';
        throw e;
    }

    return r;
}

void RobotLoader::dropRobotsToRenderer(Renderer &renderer){
    for (auto &robot : robots){
        robot->dropToRenderer(renderer);
    }
}

void RobotLoader::resetRobotPoses(const Map &worldmap)
{
    for (std::size_t i = 0; i < robots.size(); i++){
        sf::Vector2f pos = worldmap.getRandomFreeCase();
        robots[i]->setPos(pos);
    }
}

const sf::Vector2f &RobotLoader::getRobotPos(size_t robot_index)
{
    if (static_cast<int>(robot_index)>=0 && robot_index<robots.size())return robots[robot_index]->getPos();
    else {
        std::cerr << "error get robot pos , robot " << robot_index << " does not exist : they are actually " << robots.size() << " robots\n";
    }
    return robots.back()->getPos();
}

void RobotLoader::updateRobotsImg(){
    for (std::size_t i = 0; i < robots.size(); i++){
        robots[i]->updateFrame();
    }
}

void RobotLoader::updateScripts(float dt, Map &arenamap){
    std::vector<std::reference_wrapper<ScriptCommand>> commands;
    for (std::size_t i = 0; i < robots.size(); i++){
        commands.clear();
        robots[i]->initScriptVars(arenamap, dt);

        robots[i]->getScriptCommands(6, commands);
        robots[i]->updateMissiles(dt, arenamap);
        //std::cerr << "start !!\n";
        for (std::size_t c = 0; c < commands.size(); c++){
            ScriptCommand &command = commands[c].get();
            std::string type = command.getType();
            if (type == "varset")std::cerr << "runtime command " << command.getStringProp("varname") << " = " << command.getStringProp("expr") << "\n";
            else std::cerr << "runtime command type : " << type << "\n";
            command.setProp("used", 1);
            if (type == "move"){
                command.update();
                sf::Vector2f pos = robots[i]->getPos();

                sf::Vector2f off;
                off.x = static_cast<float>(command.getProp("offsetx"));
                off.y = static_cast<float>(command.getProp("offsety"));

                sf::FloatRect rectrobot(pos.x, pos.y, TILE_SIZE, TILE_SIZE);
                sf::Vector2f newpos = arenamap.collide(rectrobot, off);

                robots[i]->setPos(newpos);

                robots[i]->setScriptVar("posx", robots[i]->getPos().x);robots[i]->setScriptVar("posy", robots[i]->getPos().y);
            }else if (type == "shootmissile"){
                command.update();
                robots[i]->shootMissile(static_cast<int>(command.getProp("offsetx")));
            }else if (type == "shootguidedmissile"){
                idtype id = 0;
                id = robots[i]->shootGuidedMissile();
                ScriptBlock &block = command.getBlock();
                block.addVar("return", id);
            }else if (type == "gmissileturn"){
                command.update();
                idtype id = static_cast<idtype>(command.getProp("id"));
                int angle = static_cast<int>(command.getProp("angle"));
                robots[i]->setGMissileAngle(id, angle);
            }else if (type == "getgmissilex"){
                command.update();
                idtype id = static_cast<idtype>(command.getProp("id"));
                double x = 0;
                x = robots[i]->getGuidedMissileX(id);
                ScriptBlock &block = command.getBlock();
                block.addVar("return", x);
            }else if (type == "getgmissiley"){
                command.update();
                idtype id = static_cast<idtype>(command.getProp("id"));
                double y = 0;
                y = robots[i]->getGuidedMissileY(id);
                ScriptBlock &block = command.getBlock();
                block.addVar("return", y);
            }else if (type == "destroyblock"){
                command.update();
                int block = static_cast<int>(command.getProp("block"));
                if (block > 4 || block < 0)std::cerr << "error block " << block << " is invalid !!\n";
                int x = static_cast<int>(robots[i]->getPos().x+TILE_SIZE/2);
                int y = static_cast<int>(robots[i]->getPos().y+TILE_SIZE/2);
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
                robots[i]->initScriptVars(arenamap, dt);
            }else if (type == "createblock"){
                command.update();
                int blockrel = static_cast<int>(command.getProp("blockrel"));
                if (blockrel > 4 || blockrel < 0)std::cerr << "error block " << blockrel << " is invalid !!\n";
                int x = static_cast<int>(robots[i]->getPos().x+TILE_SIZE/2);
                int y = static_cast<int>(robots[i]->getPos().y+TILE_SIZE/2);
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

                int blockid = static_cast<int>(command.getProp("blockid"));

                arenamap.setTile(x, y, blockid);
                robots[i]->initScriptVars(arenamap, dt);
            }else if (type == "print"){
                command.update();
                std::cerr << robots[i]->getName() << " >> " << command.getStringProp("str") << '\n';
            }else if (type == "varset"){
                command.update();
                //robots[i].setScriptVar(command.getStringProp("varname"), command.getProp("val"));
            }else if (type.rfind("blockentry", 0) == 0){
                command.update();
                if (!static_cast<bool>(command.getProp("canenter"))){
                    std::cerr << "block entry failed!!\n";
                    c += command.getProp("nbcmd");
                    //std::cerr << "prop : " << command.getProp("nbcmd") << "\n";
                }
            }
        }//commands loop
        if (commands.size()>0)std::cerr << "runtime finished*******\n\n";
    }//robots loop
}
