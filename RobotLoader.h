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

class RobotLoader
{
public:
    RobotLoader();
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
    void updateScripts(float dt);

private:
    std::vector<Robot> robots;
};

#endif // ROBOTLOADER_H
