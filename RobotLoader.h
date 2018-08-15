#ifndef ROBOTLOADER_H
#define ROBOTLOADER_H

#include <iostream>
#include <experimental/filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "Robot.h"
#include "Renderer.h"
#include "ScriptCommand.h"
#include "Map.h"

namespace fs =  std::experimental::filesystem::v1;

class RobotLoader
{

public:



    RobotLoader(){}

    template<typename F>
    void loadRobots(const Map &worldmap, F &PosesRandomizer){
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
            int nb_frames;
            while (continuer && !file_opt.eof()){
                file_opt.getline(buffer, sizeof(buffer));
                std::string line(buffer);
                if (line.rfind("author:", 0) == 0) {
                    author = line.substr(7, line.size()-7);
                    std::cout << "author : " << author << '\n';
                }else if (line.rfind("nb_frames:", 0) == 0) {
                    std::string tmpstr = line.substr(10, line.size()-7);
                    try {
                        nb_frames = std::stoi(tmpstr);
                    }catch (std::exception &e){
                        std::cout << "error converting string " << tmpstr << " to int : " << e.what() << '\n';;
                    }
                    std::cout << "nb_frames : " << nb_frames << '\n';
                }
            }

            if (std::isnan(nb_frames))continue;
            robots.emplace_back(author, nb_frames, dirs[i], PosesRandomizer(), worldmap);
            std::cout << '\n';
        }
    }



    virtual ~RobotLoader();

    void dropRobotsToRenderer(Renderer &renderer);
    template<typename F>
    void setRobotPoses(F &PosesRandomizer){
        for (std::size_t i = 0; i < robots.size(); i++){
            sf::Vector2f pos = PosesRandomizer();
            robots[i].setPos(pos);
        }
    }
    void updateRobotsImg();
    void updateScripts(float dt, Map &arenamap);
    const Robot &getRobot(std::size_t index);
    std::vector<std::string> get_directories(const std::string& s);

private:
    std::vector<Robot> robots;
};

#endif // ROBOTLOADER_H
