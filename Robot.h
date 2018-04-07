#ifndef ROBOT_H
#define ROBOT_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

#include "Renderer.h"
#include "ScriptReader.h"


class Robot
{
public:
    Robot(const std::string &author, int nb_frames, const std::string &dir);
    virtual ~Robot();
    void dropToRenderer(Renderer &renderer);
    void setPos(const sf::Vector2f &pos);
    const sf::Vector2f &getPos();
    void updateFrame();
    const std::string &getPath();
    void initReader(Robot *robot);
    void getScriptCommands(std::size_t start, std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands);
    const vartype getScriptVar(const std::string &var);
    int scriptNbCommands();
    void move();
    void setScriptVar(const std::string &var, float val);

    void displayScriptVars();

private:
    std::string author;
    std::string path;
    int nb_frames;
    sf::Texture texture;
    sf::IntRect rect;
    int id_img;
    void updateRect();
    int tile_size;
    sf::Vector2f pos;
    sf::Clock imgClock;
    ScriptReader reader;
};

#endif // ROBOT_H
