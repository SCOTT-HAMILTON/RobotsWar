#ifndef ROBOT_H
#define ROBOT_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

#include "Renderer.h"
#include "ScriptReader.h"
#include "Map.h"

class Robot
{
public:
    Robot(const std::string &author, int nb_frames, const std::string &dir, const sf::Vector2f &pos, const Map &worldmap);
    virtual ~Robot();
    void dropToRenderer(Renderer &renderer);
    void setPos(const sf::Vector2f &pos);
    const sf::Vector2f &getPos() const;
    void updateFrame();
    const std::string &getPath();
    void getScriptCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands);
    double getScriptVar(const std::string &var);
    const std::string &getName();
    int scriptNbCommands();
    void move();
    void setScriptVar(const std::string &var, float val);
    void initScriptVars(const Map &worldmap);
    void displayScriptVars();

private:
    std::string author;
    std::string path;
    std::string name;
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
