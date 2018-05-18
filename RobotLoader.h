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

class RobotLoader
{
public:
    template<typename F>
    RobotLoader(const Map &worldmap, F &PosesRandomizer){
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
            std::cout << "hey !!" << std::endl;
            robots.emplace_back(author, nb_frames, dirs[i], PosesRandomizer(), worldmap);
            std::cout << "hey 2!!" << std::endl;
            std::cout << std::endl;

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
