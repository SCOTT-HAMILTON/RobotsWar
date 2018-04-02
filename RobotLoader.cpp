#include "RobotLoader.h"
#include <cstdlib>
#include <cmath>

namespace fs =  std::experimental::filesystem::v1;

std::vector<std::string> get_directories(const std::string& s)
{
    std::vector<std::string> r;
    for(auto& p : fs::recursive_directory_iterator(s))
        if(p.status().type() == fs::file_type::directory)
            r.push_back(p.path().string());
    return r;
}

RobotLoader::RobotLoader()
{
    std::string file_path;
    std::vector<std::string> dirs = get_directories("robots");
    for (std::size_t i = 0; i < dirs.size(); i++){
        std::ifstream file_opt(dirs[i]+"/prop.txt");
        if (!file_opt.good())continue;
        bool continuer = true;
        char buffer[1000];
        std::string author;
        int nb_frames;
        while (continuer && !file_opt.eof()){
            file_opt.getline(buffer, sizeof(buffer));
            std::string line(buffer);
            std::cout << line << std::endl;
            if (line.rfind("author:", 0) == 0) {
                author = line.substr(7, line.size()-7);
                std::cout << "author : " << author << std::endl;
            }else if (line.rfind("nb_frames:", 0) == 0) {
                nb_frames = std::atoi(line.substr(10, line.size()-7).c_str());
                std::cout << "nb_frames : " << nb_frames << std::endl;
            }
        }
        if (std::isnan(nb_frames))continue;

        robots.emplace_back(author, nb_frames, dirs[i]);

        std::cout << std::endl;

    }
}

RobotLoader::~RobotLoader()
{
    //dtor
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

void RobotLoader::updateScripts(float dt){
    std::vector<std::weak_ptr<ScriptCommand>> commands;
    for (std::size_t i = 0; i < robots.size(); i++){
        commands.clear();
        robots[i].getScriptCommands(0, robots[i].scriptNbCommands(), commands);
        for (std::size_t c = 0; c < commands.size(); c++){
            std::shared_ptr<ScriptCommand> command = commands[c].lock();
            if (command->getType() == "move"){
                sf::Vector2f pos = robots[i].getPos();
                float offsetx = *(const float *)command->getProp("offsetx");
                float offsety = *(const float *)command->getProp("offsety");
                if (std::isnan(offsetx)){
                    std::string varname = command->getStringProp("offsetx");
                    offsetx = *(const float *)robots[i].getScriptVar(varname);
                }
                if (std::isnan(offsety)){
                    std::string varname = command->getStringProp("offsety");
                    offsety = *(const float *)robots[i].getScriptVar(varname);
                }
                pos.x += offsetx*dt;
                pos.y += offsety*dt;
                robots[i].setPos(pos);
            }
        }
    }
}
