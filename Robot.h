#ifndef ROBOT_H
#define ROBOT_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>

#include "Renderer.h"
#include "ScriptReader.h"
#include "Map.h"

#ifndef M_PI

#define M_PI 3.14159265359

#endif

struct Missile{
    sf::Vector2f pos;
    int direction;
    bool dying;
    sf::Clock cooldown_death;
};

class Robot
{
public:
    Robot(const std::string &author, int nb_frames, const std::string &dir, const sf::Vector2f &pos, const Map &worldmap);
    virtual ~Robot();
    void dropToRenderer(Renderer &renderer);
    void setPos(const sf::Vector2f &pos);
    void updateMissiles(float dt, const Map &arenamap);
    const sf::Vector2f &getPos() const;
    void updateFrame();
    const std::string &getPath();
    void getScriptCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands);
    double getScriptVar(const std::string &var);
    const std::string &getName();
    int scriptNbCommands();
    void move();
    void setScriptVar(const std::string &var, float val);
    void initScriptVars(const Map &worldmap, float dt);
    void displayScriptVars();
    void setGMissileAngle(idtype id, int angle);
    double getGuidedMissileX(idtype id) const;
    double getGuidedMissileY(idtype id) const;

    static void initTexts();

    void shootMissile(int direction);
    idtype shootGuidedMissile();

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
    static std::map<int, sf::Texture> missiletexts;
    static std::map<int, sf::Texture> guidedmissiletexts;
    static sf::IntRect missilesize;
    static sf::IntRect guidedmissilesize;
    static float missilespeed;
    static int msTimeDeathMissile;
    std::vector<Missile> missiles;
    idtype curGuidedMissileId;
    std::map<idtype, Missile> guidedmissiles;
};

#endif // ROBOT_H
