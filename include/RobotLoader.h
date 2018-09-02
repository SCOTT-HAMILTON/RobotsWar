#ifndef ROBOTLOADER_H
#define ROBOTLOADER_H

#include <iostream>
#include <experimental/filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "Robot.h"
#include "Rendering/Renderer.h"
#include "Commands/ScriptCommand.h"
#include "Rendering/Map.h"

namespace fs =  std::experimental::filesystem;

class RobotLoader
{

public:
    RobotLoader();
    virtual ~RobotLoader();

    void loadRobots(const Map &worldmap);
    void dropRobotsToRenderer(Renderer &renderer);
    void resetRobotPoses(const Map &worldmap);
    const sf::Vector2f &getRobotPos(size_t robot_index);
    void updateRobotsImg();
    void updateScripts(float dt, Map &arenamap);
    std::vector<std::string> get_directories(const std::string& s);

private:
    std::vector<std::unique_ptr<Robot>> robots;
};

#endif // ROBOTLOADER_H
