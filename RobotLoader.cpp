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
                std::string tmpstr = line.substr(10, line.size()-7);
                try {
                    nb_frames = std::stoi(tmpstr);
                }catch (std::exception &e){
                    std::cout << "error converting string " << tmpstr << " to int : " << e.what() << std::endl;;
                }
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
        robots[i].getScriptCommands(5, commands);
        for (std::size_t c = 0; c < commands.size(); c++){
            std::shared_ptr<ScriptCommand> command = commands[c].lock();
            std::string type = command->getType();
            //std::cout << "type cmd : " << type << std::endl;
            if (type == "move"){
                command->update();
                sf::Vector2f pos = robots[i].getPos();
                double offsetx = command->getProp("offsetx");
                double offsety = command->getProp("offsety");
                pos.x += offsetx;
                pos.y += offsety;
                robots[i].setPos(pos);
            }
        }
    }
}
